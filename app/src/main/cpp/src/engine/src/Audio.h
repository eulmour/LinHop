#ifndef ENGINE_AUDIO_H
#define ENGINE_AUDIO_H

#define AUDIO_MAX_SOURCES 10

#include "Framework.h"
#include "File.h"
#include <vector>
#include <memory>

namespace wuh {

// struct AudioSource {

//     enum state state;
//     float vol;
//     File file;
//     int16_t* data;

//     size_t id;
//     size_t size;
//     size_t samples;
//     size_t position;
//     size_t num_of_ch;
//     size_t samples_per_s;
//     size_t bits;

//     AudioSourceInternal* internal;

//     AudioSource() = delete;
//     AudioSource(const char* path, float vol);
//     ~AudioSource();
// };

// class Audio {

// public:

//     // using Playlist = std::vector<AudioSource*>;
//     using Playlist = std::array<AudioSource*, AUDIO_MAX_SOURCES>;

//     Audio();
//     ~Audio();
//     void play(AudioSource& source);
//     void pause(AudioSource& source);
//     void stop(AudioSource& source);
//     void resume();
//     void pause();
//     void suspend();
//     Playlist& getPlaylist() { return this->playlist; }
//     AudioInternal* getInternal() { return this->internal; }

// protected:
//     enum state state;
//     AudioInternal* internal;
//     Playlist playlist;
//     float master_vol;
//     // size_t free_slot;
//     // short buffer[512][2];
//     // AudioSource* sources[AUDIO_MAX_SOURCES];
// };

// struct AudioSourceInternal;

struct Audio {

    Audio();
    ~Audio();

    struct Internal;

    struct Source {
        Source(std::string path, float volume);
        Source(Source&& other);
        ~Source();
        void volume(float volume) { volume_ = volume; }

        friend Audio::Internal;
        struct Internal;
    protected:
        Internal* internal_{ nullptr };
        // std::string path_{};
        float volume_{ .75f };
        bool end_{ false };
    };

    void play(Source& source);

    void resume();
    void pause();
    void suspend();
protected:

    bool active_{ false };
    std::vector<Source*> playlist_;
    Internal* internal_;
};


} // end of namespace wuh

#endif //ENGINE_AUDIO_H
