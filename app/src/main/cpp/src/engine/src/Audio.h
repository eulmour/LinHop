#ifndef ENGINE_AUDIO_H
#define ENGINE_AUDIO_H

#define AUDIO_MAX_SOURCES 10

#include "Framework.h"
#include "File.h"
#include <vector>
#include <memory>

namespace wuh {

struct AudioInternal;
struct AudioSourceInternal;

struct AudioSource {

    enum state state;
    float vol;
    File file;
    int16_t* data;

    size_t id;
    size_t size;
    size_t samples;
    size_t position;
    size_t num_of_ch;
    size_t samples_per_s;
    size_t bits;

    AudioSourceInternal* internal;

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
    // void* getInternal() { return this->internal.get(); }
    AudioInternal* getInternal() { return this->internal; }

protected:
    // std::shared_ptr<void> internal;
    AudioInternal* internal;
    enum state state;
    // size_t free_slot;
    Playlist playlist;
    // AudioSource* sources[AUDIO_MAX_SOURCES];
    float master_vol;
    // short buffer[512][2];
};

} // end of namespace wuh

#endif //ENGINE_AUDIO_H
