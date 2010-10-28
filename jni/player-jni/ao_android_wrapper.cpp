
#include <media/AudioSystem.h>
#include <media/AudioTrack.h>

#include "utility.h"

using namespace android;

static AudioTrack* audio = 0;

extern "C" {

int audio_track_create(int rate, int format, int channel) {
    switch(format) {
        case 0: // SAMPLE_FMT_U8
            format = AudioSystem::PCM_8_BIT;
            break;
        case 1: // SAMPLE_FMT_S16
            format = AudioSystem::PCM_16_BIT;
            break;
        default:
            debug("audio track format %d is not supported\n", format);
            if (audio) {
                delete audio;
                audio = 0;
            }
            return -1;
    }
    if (channel != 1 && channel != 2) {
        debug("audio track channel %d is not supported\n", channel);
        if (audio) {
            delete audio;
            audio = 0;
        }
        return -1;
    }
    if (!audio) {
        audio = new AudioTrack();
        // try donut
        status_t status;
        status = audio->set(
            AudioSystem::MUSIC,
            rate,
            format,
            channel,
            4096 * 4
        );
        // try eclair or higher
        if (status == BAD_VALUE) {
            status = audio->set(
                AudioSystem::MUSIC,
                rate,
                format,
                channel == 1 ? 0x04 : 0x0c, // CHANNEL_OUT_MONO or CHANNEL_OUT_STEREO
                4096 * 4
            );
        }
        if (status == NO_ERROR) {
            audio->start();
            debug("Got AudioTrack! %p", audio);
        }
        else {
            delete audio;
            audio = 0;
        }
    }
    return audio ? 0 : -1;
}

void audio_track_play(const void* buffer, int length) {
    int l = 0;

    while (l < length) {
        l += audio->write((char*)(buffer) + l, length - l);
    }
}

void audio_track_destroy() {
    if (audio) {
        delete audio;
        audio = 0;
    }
}

}


