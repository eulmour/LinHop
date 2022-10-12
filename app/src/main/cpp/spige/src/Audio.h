#ifndef SPIGE_AUDIO_H
#define SPIGE_AUDIO_H

#define AUDIO_MAX_SOURCES 10

#include "Framework.h"
#include <vector>
#include <memory>

struct AudioSource {

    enum state state{STATE_OFF};
    float vol;
    struct file file_data;
    int16_t* data;

    size_t id;
    size_t size;
    size_t samples;
    size_t position;
    size_t num_of_ch;
    size_t samples_per_s;
    size_t bits;
    std::shared_ptr<void> internal;

    AudioSource() = delete;
    AudioSource(const char* path, float vol);
    ~AudioSource();
};

class Audio {

public:

    using Playlist = std::vector<AudioSource*>;

    Audio();
    ~Audio();
    void play(AudioSource& source);
    void pause(AudioSource& source);
    void stop(AudioSource& source);
    void playAll();
    void pauseAll();
    void stopAll();
    Playlist& getPlaylist() { return this->playlist; }
    void* getInternal() { return this->internal.get(); }

protected:
    std::shared_ptr<void> internal;
    enum state state;
    // size_t free_slot;
    Playlist playlist;
    // AudioSource* sources[AUDIO_MAX_SOURCES];
    float master_vol;
    // short buffer[512][2];
};

#endif //SPIGE_AUDIO_H
