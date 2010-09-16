
#ifndef PLAYER_H
#define PLAYER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>

typedef struct {
    // number of streams
    int audio;
    int video;
    int subtitle;

    // preferred stream
    int preferred_audio;
    int preferred_video;
    int preferred_subtitle;

    //
    int audio_enabled;
    int video_enabled;
    int subtitle_enabled;

    // actual stream
    int audio_st_idx;
    int video_st_idx;
    int subtitle_st_idx;

    AVStream* audio_st;
    AVStream* video_st;
    AVStream* subtitle_st;

    // duration in seconds
    int64_t duration;

    // fps
    double fps;

    // video size
    int width;
    int height;

    // audio sample rate
    int rate;
    int channel;

    // runtime parameters
    AVFormatContext* av_ctx;
    AVCodecContext* audio_ctx;
    AVCodec* audio_codec;
    AVCodecContext* video_ctx;
    AVCodec* video_codec;
    AVCodecContext* subtitle_ctx;
    AVCodec* subtitle_codec;

    AVFrame* frame;
    uint16_t* samples;

    double audio_time_base;
    double video_time_base;
    double subtitle_time_base;
    int64_t audio_last_pts;
    int64_t video_last_pts;
    int64_t subtitle_last_pts;

    // 0 = centeral surface if possible
    // 1 = fit surface
    // 2 = full surface
    int mode;
    int exit;
    int pause;
    int step;
}PlayerCtx;

extern PlayerCtx* gCtx;

#ifdef __cplusplus
}
#endif

#endif

