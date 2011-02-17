
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <vlc_common.h>
#include <vlc_plugin.h>
#include <vlc_aout.h>

#include <media/AudioSystem.h>
#include <media/AudioTrack.h>

#ifndef __PLATFORM__
#error "android api level is not defined!"
#endif

using namespace android;

struct aout_sys_t {
    int type;
    int rate;
    int channel;
    int format;
    int size;
    AudioTrack *handle;
};

static int  Open(vlc_object_t *);
static void Close(vlc_object_t *);
static void Play(aout_instance_t *);

vlc_module_begin ()
    set_shortname("AndroidAudioTrack")
    set_description(N_("Android AudioTrack audio output"))
    set_capability("audio output", __PLATFORM__)
    set_category(CAT_AUDIO)
    set_subcategory(SUBCAT_AUDIO_AOUT)
    add_shortcut("android")
    set_callbacks(Open, Close)
vlc_module_end ()

static int Open(vlc_object_t *p_this) {
    struct aout_sys_t *p_sys;
    aout_instance_t *p_aout = (aout_instance_t*)(p_this);
    status_t status;
#if __PLATFORM__ < 9
    int afSampleRate, afFrameCount, afLatency, minBufCount, minFrameCount;
#endif
    int type, channel, rate, format, size;

    p_sys = (struct aout_sys_t*)malloc(sizeof(aout_sys_t));
    if (p_sys == NULL)
        return VLC_ENOMEM;
    p_aout->output.p_sys = p_sys;
    type = AudioSystem::MUSIC;
    p_sys->type = type;
    // 8000 <= frequency <= 48000
    if (p_aout->output.output.i_rate < 8000)
        p_aout->output.output.i_rate = 8000;
    if (p_aout->output.output.i_rate > 48000)
        p_aout->output.output.i_rate = 48000;
    rate = p_aout->output.output.i_rate;
    p_sys->rate = rate;
    // U8/S16 only
    if (p_aout->output.output.i_format != VLC_CODEC_U8 && p_aout->output.output.i_format != VLC_CODEC_S16L)
        p_aout->output.output.i_format = VLC_CODEC_S16L;
    format = (p_aout->output.output.i_format == VLC_CODEC_S16L) ? AudioSystem::PCM_16_BIT : AudioSystem::PCM_8_BIT;
    p_sys->format = format;
    // TODO: android supports more channels
    channel = aout_FormatNbChannels(&p_aout->output.output);
    if (channel > 2) {
        channel = 2;
        p_aout->output.output.i_physical_channels = AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT;
    }
#if __PLATFORM__ >= 5
    channel = (channel == 2) ? AudioSystem::CHANNEL_OUT_STEREO : AudioSystem::CHANNEL_OUT_MONO;
#endif
    p_sys->channel = channel;
    // use the minium value
#if __PLATFORM__ < 9
    status = AudioSystem::getOutputSamplingRate(&afSampleRate, type);
    status ^= AudioSystem::getOutputFrameCount(&afFrameCount, type);
    status ^= AudioSystem::getOutputLatency((uint32_t*)(&afLatency), type);
    if (status != NO_ERROR) {
        msg_Err(p_aout, "AudioSystem is not initialized!");
        free(p_sys);
        return VLC_EGENERIC;
    }
    minBufCount = afLatency / ((1000 * afFrameCount) / afSampleRate);
    if (minBufCount < 2)
        minBufCount = 2;
    minFrameCount = (afFrameCount * rate * minBufCount) / afSampleRate;
    p_aout->output.i_nb_samples = minFrameCount;
#else
    status = AudioTrack::getMinFrameCount(&p_aout->output.i_nb_samples, type, rate);
    if (status != NO_ERROR) {
        msg_Err(p_aout, "AudioTrack::getMinFrameCount() failed!");
        free(p_sys);
        return VLC_EGENERIC;
    }
#endif
    p_aout->output.i_nb_samples <<= 1;
    p_sys->size = p_aout->output.i_nb_samples;
    p_sys->handle = new AudioTrack(p_sys->type, p_sys->rate, p_sys->format, p_sys->channel, p_sys->size);
    status = p_sys->handle->initCheck();
    if (status != NO_ERROR) {
        msg_Err(p_aout, "Cannot create AudioTrack!");
        delete p_sys->handle;
        free(p_sys);
        return VLC_EGENERIC;
    }
    p_sys->handle->start();
    p_aout->output.pf_play = Play;
    return VLC_SUCCESS;
}

static void Close(vlc_object_t *p_this) {
    aout_instance_t *p_aout = (aout_instance_t*)p_this;
    struct aout_sys_t *p_sys = p_aout->output.p_sys;

    p_sys->handle->stop();
    p_sys->handle->flush();
    delete p_sys->handle;
    vlc_object_kill(p_aout);
    vlc_thread_join(p_aout);
    p_aout->b_die = false;
    free(p_sys);
}

static void Play(aout_instance_t *p_aout) {
    struct aout_sys_t *p_sys = p_aout->output.p_sys;
    int length;
    aout_buffer_t *p_buffer;

    while (vlc_object_alive(p_aout)) {
        p_buffer = aout_FifoPop(p_aout, &p_aout->output.fifo);
        if (p_buffer != NULL) {
            length = 0;
            while (length < p_buffer->i_buffer) {
                length += p_sys->handle->write((char*)(p_buffer->p_buffer) + length, p_buffer->i_buffer - length);
            }
            aout_BufferFree(p_buffer);
        }
        else
            break;
    }
}

