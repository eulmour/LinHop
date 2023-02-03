#include "Audio.h"
#include <memory.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <memory>
#include <exception>
#include <stdexcept>

#define AUDIO_NUM_CHANNELS 2
#define AUDIO_SAMPLE_FORMAT ma_format_s16
#define AUDIO_BITS_PER_SAMPLE 16
#define AUDIO_SAMPLE_RATE 48000
#define AUDIO_BUFFER_SIZE 512

#include "miniaudio.h"

namespace wuh {

// static ma_uint32 read_and_mix_pcm_frames_s16(ma_decoder* pDecoder, int16_t* pOutputS16, ma_uint32 frameCount) {
//     /*
//     The way mixing works is that we just read into a temporary buffer, then take the contents of that buffer and mix it with the
//     contents of the output buffer by simply adding the samples together. You could also clip the samples to -1..+1, but I'm not
//     doing that in this example.
//     */
//     ma_result result;
//     int16_t temp[4096];
//     ma_uint32 tempCapInFrames = (sizeof(temp)/sizeof(temp[0])) / AUDIO_NUM_CHANNELS;
//     ma_uint32 totalFramesRead = 0;

//     while (totalFramesRead < frameCount) {
//         ma_uint64 iSample;
//         ma_uint64 framesReadThisIteration;
//         ma_uint32 totalFramesRemaining = frameCount - totalFramesRead;
//         ma_uint32 framesToReadThisIteration = tempCapInFrames;
//         if (framesToReadThisIteration > totalFramesRemaining) {
//             framesToReadThisIteration = totalFramesRemaining;
//         }

//         result = ma_decoder_read_pcm_frames(pDecoder, temp, framesToReadThisIteration, &framesReadThisIteration);
//         if (result != MA_SUCCESS || framesReadThisIteration == 0) {
//             break;
//         }

//         /* Mix the frames together. */
//         for (iSample = 0; iSample < framesReadThisIteration*AUDIO_NUM_CHANNELS; ++iSample) {
//             pOutputS16[totalFramesRead*AUDIO_NUM_CHANNELS + iSample] += temp[iSample];
//         }

//         totalFramesRead += (ma_uint32)framesReadThisIteration;

//         if (framesReadThisIteration < (ma_uint32)framesToReadThisIteration) {
//             break;  /* Reached EOF. */
//         }
//     }
    
//     return totalFramesRead;
// }

// static void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {

//     auto* pOutputS16 = (int16_t*)pOutput;
//     auto* audio = reinterpret_cast<Audio*>(pDevice->pUserData);
//     auto* internal_ptr = reinterpret_cast<AudioInternal*>(audio->getInternal());

//     // MA_ASSERT(pDevice->playback.format == AUDIO_SAMPLE_FORMAT);   /* <-- Important for this example. */

//     auto playlist = audio->getPlaylist();
//     for (std::size_t i = 0; i < playlist.size(); ++i) {

//         if (playlist[i]->state == STATE_READY)
//             continue;

//         // auto* source_internal_ptr = reinterpret_cast<AudioSourceInternal*>(playlist[i]->internal.get());
//         auto* source_internal_ptr = playlist[i]->internal;

//         playlist[i]->state = STATE_BUSY;
//         ma_uint32 framesRead = read_and_mix_pcm_frames_s16(
//                 &source_internal_ptr->decoder, pOutputS16, frameCount);

//         if (framesRead < frameCount) {
//             playlist[i]->state = STATE_READY;
//             playlist.erase(playlist.begin() + i);
//         }
//     }

//     /*
//     If at the end all of our decoders are at the end we need to stop. We cannot stop the device in the callback. Instead we need to
//     signal an event to indicate that it's stopped. The main thread will be waiting on the event, after which it will stop the device.
//     */
//     bool the_end = true;
//     for (const auto source : audio->getPlaylist()) {
//         if (source->state == STATE_BUSY)
//             the_end = false;
//     }

//     if (the_end)
//         ma_event_signal(&internal_ptr->g_stopEvent);

//     (void)pInput;
// }

// AudioSource::AudioSource(const char* path, float vol) : file(path) {

//     this->vol = vol;

//     this->internal = new AudioSourceInternal();
//     auto* internal_ptr = this->internal;

//     // this->internal = std::shared_ptr<void>(reinterpret_cast<void*>(new AudioSourceInternal()));
//     // auto* internal_ptr = static_cast<AudioSourceInternal*>(this->internal.get());

//     ma_result result;

//      /* In this example, all decoders need to have the same output format. */
//     internal_ptr->decoderConfig = ma_decoder_config_init(AUDIO_SAMPLE_FORMAT, AUDIO_NUM_CHANNELS, AUDIO_SAMPLE_RATE);

//     result = ma_decoder_init_memory(
//         this->file.data(),
//         this->file.size(),
//         &internal_ptr->decoderConfig,
//         &internal_ptr->decoder);

//     if (result != MA_SUCCESS) {
//         throw std::runtime_error("Could not init audio source from file");
//     }
// }

// AudioSource::~AudioSource() {
//     // auto* internal_ptr = static_cast<AudioSourceInternal*>(this->internal.get());
//     ma_decoder_uninit(&this->internal->decoder);
// }

// Audio::Audio() {

//     // this->internal = std::make_unique<AudioInternal>();
//     // auto* internal_ptr = static_cast<AudioInternal*>(this->internal.get());
//     this->internal = new AudioInternal();
//     auto* internal_ptr = this->internal;

//     /* Create only a single device. The decoders will be mixed together in the callback. In this example the data format needs to be the same as the decoders. */
//     internal_ptr->deviceConfig = ma_device_config_init(ma_device_type_playback);
//     internal_ptr->deviceConfig.playback.format   = AUDIO_SAMPLE_FORMAT;
//     internal_ptr->deviceConfig.playback.channels = AUDIO_NUM_CHANNELS;
//     internal_ptr->deviceConfig.sampleRate        = AUDIO_SAMPLE_RATE;
//     internal_ptr->deviceConfig.dataCallback      = data_callback;
//     internal_ptr->deviceConfig.pUserData         = internal_ptr;

//     if (ma_device_init(nullptr, &internal_ptr->deviceConfig, &internal_ptr->device) != MA_SUCCESS)
//         throw std::runtime_error("Failed to open playback device");

//     this->state = STATE_READY;
// }

// Audio::~Audio() {
//     // auto* internal_ptr = static_cast<AudioInternal*>(this->internal.get());
//     ma_device_uninit(&this->internal->device);
// }

// void Audio::play(AudioSource& source) {

//     return;
//     // TODO unable to read memory from this ptr
//     // auto* internal_ptr = static_cast<AudioInternal*>(source.internal.get());
//     auto* internal_ptr = this->internal;

//     this->playlist.push_back(&source);

//     if (this->state == STATE_READY) {

//         /*
//         We can't stop in the audio thread so we instead need to use an event. We wait on this thread in the main thread, and signal it in the audio thread. This
//         needs to be done before starting the device. We need a context to initialize the event, which we can get from the device. Alternatively you can initialize
//         a context separately, but we don't need to do that for this example.
//         */
//         ma_event_init(&internal_ptr->g_stopEvent);

//         /* Now we start playback and wait for the audio thread to tell us to stop. */
//         if (ma_device_start(&internal_ptr->device) != MA_SUCCESS) {
//             this->state = STATE_ERROR;
//             throw std::runtime_error("Failed to start playback device");
//         }

//         this->state = STATE_BUSY;
//         LOGV("Waiting for playback to complete...\n");
//         ma_event_wait(&internal_ptr->g_stopEvent);

//     } else if (this->state == STATE_OFF || this->state == STATE_ERROR) {
//         throw std::runtime_error("Failed to play audio, context is not ready");
//     }
// }

// void Audio::pause(AudioSource& source) {
//     (void)source;
// }

// void Audio::stop(AudioSource& source) {
//     (void)source;
// }

// void Audio::resume() {}

// void Audio::pause() {}

// void Audio::suspend() {}

static constexpr ma_format format = ma_format_s16;
static constexpr ma_uint32 channels = 2;
static constexpr ma_uint32 sample_rate = 48000;

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
    if (self == NULL || self->playlist_.empty()) {
        return;
    }

    static constexpr std::size_t buffer_size = 4096;

    for (Audio::Source* source : self->playlist_) {
        if (source->end_) {
            continue;
        }

        int16_t buffer[buffer_size];
        ma_uint32 total_frames_read = 0;

        while (total_frames_read < frames_count) {

            ma_uint64 frames_read;
            ma_decoder_read_pcm_frames(source->internal_->decoder.get(), buffer, frames_count, &frames_read);

            if (frames_read == 0) {
                source->end_ = true;
                break;
            }

            for (ma_uint64 i = 0; i < frames_read * channels; ++i) {
                ((int16_t*)output_ptr)[total_frames_read * channels + i] += buffer[i] * source->volume_;
            }

            total_frames_read += frames_read;
            if (frames_count > frames_read) {
                break;
            }
        }
    }

    (void)pInput;
}

Audio::Audio() {
    internal_ = new Internal();
    internal_->config = ma_device_config_init(ma_device_type_playback);
    internal_->config.playback.format   = format;
    internal_->config.playback.channels = channels;
    internal_->config.sampleRate        = sample_rate;
    internal_->config.dataCallback      = Internal::ma_data_callback_;
    internal_->config.pUserData         = this;
}

Audio::~Audio() {
    ma_device_uninit(&internal_->device);
    delete internal_;
}

Audio::Source::Source(std::string path, float volume) : volume_(volume) {
    internal_ = new Internal();
    ma_decoder_config config = ma_decoder_config_init(format, channels, sample_rate);
#ifndef NDEBUG
    if (!File::exists(path.c_str())) {
        throw std::runtime_error("Audio file does not exist");
    }
#endif
    ma_result result = ma_decoder_init_file(path.c_str(), &config, internal_->decoder.get());
    if (result != MA_SUCCESS) {
        throw std::runtime_error("Failed to initialize decoder");
    }
}

Audio::Source::Source(Source&& other) : internal_(other.internal_) { other.internal_ = nullptr; }

Audio::Source::~Source() {
    if (internal_ != nullptr) {
        ma_decoder_uninit(internal_->decoder.get());
    }
    delete internal_;
}

void Audio::play(Source& source) {

    playlist_.push_back(&source);
    // playlist_.emplace_back("audio/warning.wav");
    // playlist_.emplace_back("audio/fail2.wav");

    // if (!active_) {
        if (ma_device_init(NULL, &internal_->config, &internal_->device) != MA_SUCCESS) {
            throw std::runtime_error("Failed to open playback device, ma_device_init failed");
        }
        if (ma_device_start(&internal_->device) != MA_SUCCESS) {
            throw std::runtime_error("Failed to start playback device, ma_device_start failed");
        }
        // active_ = true;
    // }
}

void Audio::resume() {
    // active_ = true;
}
void Audio::pause() {
    // active_ = false;
}
void Audio::suspend() {
    // playlist_.clear();
    // active_ = false;
}

} // end of namespace wuh