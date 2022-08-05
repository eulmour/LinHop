#ifndef SPIGE_AUDIO_H
#define SPIGE_AUDIO_H

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#define AUDIO_MAX_SOURCES 10

#include "framework.h"

struct audio_source {
    enum state state;
    float vol;
    struct file wav_file;
    uint16_t* data;

    size_t id;
    size_t size;
    size_t samples;
    size_t position;
    size_t num_of_ch;
    size_t samples_per_s;
    size_t bits;
};

struct audio {
    enum state state;

    size_t free_slot;
    struct audio_source sources[AUDIO_MAX_SOURCES];

    float master_vol;
    short buffer[512][2];

    SLObjectItf sl_engine;
    SLEngineItf sl_engine_interface;
    SLObjectItf sl_output_mix;
    SLObjectItf sl_audio_player;
    SLPlayItf sl_audio_player_interface;
    SLBufferQueueItf sl_buffer_queue_interface;
    SLVolumeItf sl_volume_interface;
};

#ifdef __cplusplus
extern "C" {
#endif

int  audio_init(struct audio* engine);
void audio_play(struct audio* engine, struct audio_source* source);
void audio_pause(struct audio* engine, struct audio_source* source);
void audio_stop(struct audio* engine, struct audio_source* source);
void audio_play_all(struct audio* engine);
void audio_pause_all(struct audio* engine);
void audio_stop_all(struct audio* engine);
void audio_destroy(struct audio* engine);

int audio_source_load(struct audio_source* source, const char* path, float vol);
void audio_source_unload(struct audio_source* source);

//size_t audio_wav_i16i_pcm_read(void **dest, const void* source, size_t size);

#ifdef __cplusplus
}
#endif

#endif //SPIGE_AUDIO_H
