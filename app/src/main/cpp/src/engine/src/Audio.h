#ifndef ENGINE_AUDIO_H
#define ENGINE_AUDIO_H

#define AUDIO_MAX_SOURCES 10

#include "Framework.h"
#include "File.h"
#include <vector>
#include <memory>

namespace wuh {

struct Audio {

    Audio();
    ~Audio();

    struct Internal;

    struct Source {
        Source(std::string path, float volume);
        Source(Source&& other);
        ~Source();
        [[nodiscard]] std::size_t seek() const;
        void seek(std::size_t frame);
        void volume(float volume) { volume_ = volume; }

        friend Audio;
        friend Audio::Internal;
        struct Internal;
    protected:
        float volume_{ .75f };
        bool end_{ false };
        Internal* internal_{ nullptr };
    };

    void play(Source& source);

    void resume();
    [[nodiscard]] bool paused() const { return paused_; }
    void paused(bool flag);
    void suspend();
    [[nodiscard]] float volume() const { return volume_; }
    void volume(float volume) { volume_ = volume; }
protected:

    bool paused_{ false };
    float volume_{ 1.f };
    std::vector<Source*> playlist_;
    Internal* internal_{ nullptr };
};

} // end of namespace wuh

#endif //ENGINE_AUDIO_H
