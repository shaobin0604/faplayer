
#include "player.h"
#include "decode.h"
#include "output.h"
#include "queue.h"
#include "utility.h"

#include <memory.h>

PlayerCtx* gCtx = 0;

int player_open(const char* file);
void player_close();
int player_play(double start, int ast, int vst, int sst);
int player_seek(double time);
void player_pause();
void player_resume();

int player_get_duration();
double player_get_current_time();

int player_set_video_mode(int mode);

int player_get_video_width();
int player_get_video_height();

int player_get_audio_stream_count();
int player_get_video_stream_count();
int player_get_subtitle_stream_count();

int player_is_playing();

static void init_video_codec_ctx(AVCodecContext* ctx) {
    if (!ctx)
        return;
    ctx->flags |= 0;
    ctx->flags2 |= (CODEC_FLAG2_FAST | CODEC_FLAG2_FASTPSKIP);
	ctx->thread_count = 2;
	ctx->workaround_bugs = FF_BUG_AUTODETECT;
    ctx->error_recognition = FF_ER_CAREFUL;
    ctx->error_concealment = FF_EC_GUESS_MVS | FF_EC_DEBLOCK;
    ctx->debug = 0;
    ctx->debug_mv = 0;
    ctx->skip_idct = AVDISCARD_DEFAULT;
	ctx->skip_frame = AVDISCARD_DEFAULT;
}

int player_open(const char* file) {
    int i, err;

    av_register_all();

    if (gCtx) {
        player_close();
    }
    gCtx = (PlayerCtx*) av_mallocz(sizeof(PlayerCtx));
    if (!gCtx)
        return -1;
    err = av_open_input_file(&gCtx->av_ctx, file, 0, 0, 0);
    if (err < 0) {
        player_close();
        debug("av_open_input_file fail: %d\n", err);
        return -1;
    }
    gCtx->av_ctx->flags |= AVFMT_FLAG_GENPTS;
    err = av_find_stream_info(gCtx->av_ctx);
    if (err < 0) {
        player_close();
        debug("av_find_stream_info fail: %d\n", err);
        return -1;
    }
	if (gCtx->av_ctx->duration != AV_NOPTS_VALUE)
		gCtx->duration = gCtx->av_ctx->duration / AV_TIME_BASE ;
    for (i = 0; i < gCtx->av_ctx->nb_streams; i++) {
		AVStream *st = gCtx->av_ctx->streams[i];
		AVCodecContext *codec = st->codec;
        switch(codec->codec_type) {
        case CODEC_TYPE_AUDIO:
            gCtx->audio++;
            break;
        case CODEC_TYPE_VIDEO:
            if (!gCtx->video) {
                gCtx->width = codec->width;
                gCtx->height = codec->height;
            }
            gCtx->video++;
            break;
        case CODEC_TYPE_SUBTITLE:
            gCtx->subtitle++;
            break;
        default:
            break;
        }
    }

    dump_format(gCtx->av_ctx, 0, file, 0);

    return 0;
}

void player_close() {
    if (gCtx) {
        decode_free();
        input_free();
        output_free();
        queue_free();
        pthread_mutex_destroy(&gCtx->skip_mutex);
        pthread_cond_destroy(&gCtx->start_condv);
        pthread_mutex_destroy(&gCtx->start_mutex);
        if (gCtx->audio_ctx)
            avcodec_close(gCtx->audio_ctx);
        if (gCtx->video_ctx)
            avcodec_close(gCtx->video_ctx);
        if (gCtx->subtitle_ctx)
            avcodec_close(gCtx->subtitle_ctx);
        if (gCtx->frame)
            av_free(gCtx->frame);
        if (gCtx->samples)
            av_free(gCtx->samples);
        if (gCtx->av_ctx)
        	av_close_input_file(gCtx->av_ctx);
        av_free(gCtx);
        gCtx = 0;
    }
}

int player_play(double start, int audio, int video, int subtitle) {
    int i, audio_st, video_st, subtitle_st;

    if (!gCtx)
        return -1;
    if (gCtx->audio > 0 && (audio >= 0 && audio < gCtx->audio))
        gCtx->audio_enabled = -1;
    if (gCtx->video > 0 && (video >= 0 && video < gCtx->video))
        gCtx->video_enabled = -1;
    if (gCtx->subtitle > 0 && (subtitle >= 0 && subtitle < gCtx->subtitle))
        gCtx->subtitle_enabled = -1;
    gCtx->preferred_audio = gCtx->audio_enabled ? audio : -1;
    gCtx->preferred_video = gCtx->video_enabled ? video : -1;
    gCtx->preferred_subtitle = gCtx->subtitle_enabled ? subtitle : -1;
    if (gCtx->audio_enabled + gCtx->video_enabled + gCtx->subtitle_enabled == 0) {
        player_close();
        return -1;
    }
    audio_st = 0;
    video_st = 0;
    subtitle_st = 0;
    for (i = 0; i <gCtx->av_ctx->nb_streams; i++) {
		AVStream *st = gCtx->av_ctx->streams[i];
		AVCodecContext *codec = st->codec;
        switch(codec->codec_type) {
        case CODEC_TYPE_AUDIO:
            audio_st++;
            if (audio_st - 1 == gCtx->preferred_audio) {
                gCtx->audio_st = st;
                gCtx->audio_ctx = codec;
                gCtx->audio_st_idx = i;
            }
            break;
        case CODEC_TYPE_VIDEO:
            video_st++;
            if (video_st - 1 == gCtx->preferred_video) {
                gCtx->video_st = st;
                gCtx->video_ctx = codec;
                gCtx->video_st_idx = i;
            }
            break;
        case CODEC_TYPE_SUBTITLE:
            subtitle_st++;
            if (subtitle_st - 1 == gCtx->preferred_subtitle) {
                gCtx->subtitle_st = st;
                gCtx->subtitle_ctx = codec;
                gCtx->subtitle_st_idx = i;
            }
            break;
        default:
            break;
        }
    }
    if (gCtx->audio && gCtx->audio_enabled) {
        gCtx->audio_codec = avcodec_find_decoder(gCtx->audio_ctx->codec_id);
        if (!gCtx->audio_codec || avcodec_open(gCtx->audio_ctx, gCtx->audio_codec) < 0) {
            gCtx->audio_enabled = 0;
            debug("could not find/open audio codec\n");
        }
        gCtx->rate = gCtx->audio_ctx->sample_rate;
        gCtx->channel = gCtx->audio_ctx->channels;
        gCtx->audio_time_base = av_q2d(gCtx->audio_st->time_base);
    }
    if (gCtx->video && gCtx->video_enabled) {
        init_video_codec_ctx(gCtx->video_ctx);
        gCtx->video_codec = avcodec_find_decoder(gCtx->video_ctx->codec_id);
        if (!gCtx->video_codec || avcodec_open(gCtx->video_ctx, gCtx->video_codec) < 0) {
            gCtx->video_enabled = 0;
            debug("could not find/open video codec\n");
        }
        else {
            if (gCtx->video_codec->capabilities & CODEC_CAP_TRUNCATED)
                gCtx->video_ctx->flags |= CODEC_FLAG_TRUNCATED;
            if (gCtx->video_st->avg_frame_rate.den && gCtx->video_st->avg_frame_rate.num)
                gCtx->fps = av_q2d(gCtx->video_st->avg_frame_rate); // fps
            else if (gCtx->video_st->r_frame_rate.den && gCtx->video_st->r_frame_rate.num)
    		    gCtx->fps = av_q2d(gCtx->video_st->r_frame_rate); // tbr
    		else
		        gCtx->fps = 1 / av_q2d(gCtx->video_ctx->time_base); // tbn
            if (gCtx->fps <= 0 || gCtx->fps > 100)
                gCtx->fps = 24;
            gCtx->video_time_base = av_q2d(gCtx->video_st->time_base);
        }
    }
    if (gCtx->subtitle && gCtx->subtitle_enabled) {
        gCtx->subtitle_codec = avcodec_find_decoder(gCtx->subtitle_ctx->codec_id);
        if (!gCtx->subtitle_codec || avcodec_open(gCtx->subtitle_ctx, gCtx->subtitle_codec) < 0) {
            gCtx->subtitle_enabled = 0;
            debug("could not find/open subtitle codec\n");
        } else {
            gCtx->subtitle_time_base = av_q2d(gCtx->subtitle_st->time_base);
        }
    }
    if (gCtx->audio_enabled + gCtx->video_enabled + gCtx->subtitle_enabled == 0) {
        player_close();
        return -1;
    }
    if (gCtx->audio_enabled)
        debug("audio is enabled");
    if (gCtx->video_enabled)
        debug("video is enabled");
    if (gCtx->subtitle_enabled)
        debug("subtitle is enabled");
    pthread_mutex_init(&gCtx->start_mutex, 0);
    pthread_cond_init(&gCtx->start_condv, 0);
    pthread_mutex_init(&gCtx->skip_mutex, 0);
    player_seek(start);
    gCtx->frame = avcodec_alloc_frame();
    if (!gCtx->frame) {
        player_close();
        return -1;
    }
    gCtx->samples = av_malloc(AVCODEC_MAX_AUDIO_FRAME_SIZE);
    if (!gCtx->samples) {
        player_close();
        return -1;
    }
    if (queue_init() < 0) {
        player_close();
        return -1;
    }
    if (input_init() < 0) {
        player_close();
        return -1;
    }
    if (decode_init() < 0) {
        player_close();
        return -1;
    }
    if (output_init() < 0) {
        player_close();
        return -1;
    }

    return 0;
}

int player_seek(double time) {
    int err;
    int64_t timestamp;

    if (!gCtx)
        return -1;
    if ((int64_t)(time / gCtx->audio_time_base) != AV_NOPTS_VALUE) {
        gCtx->pause = -1;
        timestamp = (int64_t)(time / gCtx->audio_time_base);
        if (gCtx->av_ctx->start_time != AV_NOPTS_VALUE)
            timestamp += gCtx->av_ctx->start_time;
        err = avformat_seek_file(gCtx->av_ctx, -1, INT64_MIN, timestamp, INT64_MAX, 0);
        if (err < 0) {
            debug("avformat_seek_file failed\n");
            gCtx->pause = 0;
            return -1;
        }
        audio_packet_queue_abort();
        video_packet_queue_abort();
        subtitle_packet_queue_abort();
        samples_queue_abort();
        picture_queue_abort();
        audio_frame_queue_abort();
        video_frame_queue_abort();
        gCtx->pause = 0;
        return 0;
    }
    return 0;
}

void player_pause() {
    if (gCtx)
        gCtx->pause = -1;
}

void player_resume() {
    if (gCtx)
        gCtx->pause = 0;
}

int player_get_duration() {
    return gCtx ? gCtx->duration : 0;
}

double player_get_current_time() {
    return gCtx ? gCtx->audio_last_pts * gCtx->audio_time_base : 0;
}

int player_set_video_mode(int mode) {
    if (!gCtx || mode < 0 || mode > 2)
        return -1;
    gCtx->mode = mode;
}

int player_get_video_width() {
    return gCtx ? gCtx->width : 0;
}

int player_get_video_height() {
    return gCtx ? gCtx->height : 0;
}

int player_get_audio_stream_count() {
    return gCtx ? gCtx->audio : 0;
}

int player_get_video_stream_count() {
    return gCtx ? gCtx->video : 0;
}

int player_get_subtitle_stream_count() {
    return gCtx ? gCtx->subtitle : 0;
}

int player_is_playing() {
    return gCtx ? (gCtx->pause ? 0 : -1) : 0;
}

