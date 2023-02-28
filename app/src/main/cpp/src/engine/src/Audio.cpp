#include "Audio.h"
#include <memory.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <memory>
#include <exception>
#include <stdexcept>
#include <algorithm>

#include "miniaudio.h"

namespace wuh {

static constexpr ma_format format = ma_format_s16;
static constexpr ma_uint32 channels = 2;
static constexpr ma_uint32 sample_rate = 48000;
static constexpr ma_uint32 buffer_size = 512;

struct Audio::Internal {
    ma_device device;
    ma_device_config config;
    static void ma_data_callback_(ma_device* pDevice, void* output_ptr, const void* pInput, ma_uint32 frames_count);
};

struct Audio::Source::Internal {
    std::unique_ptr<ma_decoder> decoder{new ma_decoder};
};

void Audio::Internal::ma_data_callback_(ma_device* pDevice, void* output_ptr, const void* pInput, ma_uint32 frames_count) {

    Audio* self = (Audio*)pDevice->pUserData;
    if (self == NULL || self->paused_ || self->playlist_.empty()) {
        return;
    }
    
    for (auto it = self->playlist_.begin(); it != self->playlist_.end(); ++it) {
        auto source = *it;
        if (source->end_) {
            self->playlist_.erase(it--);
            continue;
        }

        ma_int16 buffer[buffer_size * channels];
        ma_uint64 total_frames_read = 0;

        while (total_frames_read < frames_count) {

            ma_uint64 frames_read;
            ma_uint64 frames_remaining = frames_count - total_frames_read;
            ma_uint64 frames_to_read = buffer_size;

            if (frames_to_read > frames_remaining) {
                frames_to_read = frames_remaining;
            }

            ma_decoder_read_pcm_frames(source->internal_->decoder.get(), buffer, frames_to_read, &frames_read);

            for (ma_uint64 i = 0; i < frames_read * channels; ++i) {
                ((ma_int16*)output_ptr)[total_frames_read * channels + i] += buffer[i] * (self->volume_ * source->volume_); // TODO float * i16
            }

            total_frames_read += frames_read;
            if (frames_read < frames_to_read) {
                source->end_ = true;
                break;
            }
        }
    }

    (void)pInput;
}

Audio::Audio() : paused_(true) {
    internal_ = new Internal();
    internal_->config = ma_device_config_init(ma_device_type_playback);
    internal_->config.playback.format   = format;
    internal_->config.playback.channels = channels;
    internal_->config.sampleRate        = sample_rate;
    internal_->config.dataCallback      = Internal::ma_data_callback_;
    internal_->config.pUserData         = this;
}

Audio::~Audio() {
    this->suspend();
    delete internal_;
}

Audio::Source::Source(std::string path, float volume) : volume_(volume) {
    internal_ = new Internal();
    ma_decoder_config config = ma_decoder_config_init(format, channels, sample_rate);
#ifndef NDEBUG
    if (!File::exists(path.c_str())) {
        throw std::runtime_error(path + " does not exist");
    }
#endif
    ma_result result = ma_decoder_init_file(path.c_str(), &config, internal_->decoder.get());
    if (result != MA_SUCCESS) {
        throw std::runtime_error("Failed to initialize decoder for " + path);
    }
}

Audio::Source::Source(Source&& other) : internal_(other.internal_) { other.internal_ = nullptr; }

Audio::Source::~Source() {
    if (internal_ != nullptr) {
        ma_decoder_uninit(internal_->decoder.get());
        delete internal_;
    }
}

[[nodiscard]] std::size_t Audio::Source::seek() const {
    return static_cast<std::size_t>(internal_->decoder->readPointerInPCMFrames);
}

void Audio::Source::seek(std::size_t frame) {
    end_ = false;
    ma_decoder_seek_to_pcm_frame(internal_->decoder.get(), frame);
}

void Audio::play(Source& source) {
    source.seek(0);
    playlist_.push_back(&source);
}

void Audio::resume() {

    if (ma_device_init(NULL, &internal_->config, &internal_->device) != MA_SUCCESS) {
        throw std::runtime_error("Failed to open playback device, ma_device_init failed");
    }
    if (ma_device_start(&internal_->device) != MA_SUCCESS) {
        throw std::runtime_error("Failed to start playback device, ma_device_start failed");
    }

    this->paused(false);
}

void Audio::paused(bool flag) {
    paused_ = flag;
}

void Audio::suspend() {
    this->paused(true);
    ma_device_uninit(&internal_->device);
    playlist_.clear();
}

} // end of namespace wuh