#include "audio.h"
#include <memory.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#define AUDIO_NUM_CHANNELS 2
#define AUDIO_BITS_PER_SAMPLE 16
#define AUDIO_SAMPLE_RATE 48000
#define AUDIO_BUFFER_SIZE 512

//DSCP compressor values
#define DSP_CMPR_CONST_A -0.2f
#define DSP_CMPR_CONST_B 1.1f
#define NORMALIZE_INT16(x) (2 * (x / (INT16_MAX - INT16_MIN)))

inline float dsp_compressor_func(float x) {
    return ((DSP_CMPR_CONST_B * x) + (DSP_CMPR_CONST_A * x * x * x));
}

#if defined(__ANDROID__) || defined(ANDROID)
//Callback for swapping audio buffers
void sl_buffer_callback (SLBufferQueueItf snd_queue, void *c)
{
    struct audio *e = (struct audio*)c;

    if (e->state != STATE_OFF) {

        memset(e->buffer, 0, sizeof(e->buffer));

        //Populate the current audio buffer with the whatever sounds that are playing.
        for (int i = 0; i < AUDIO_MAX_SOURCES; i++) {

            struct audio_source* source = &e->sources[i];

            if (source->state != STATE_BUSY)
                continue;

            size_t smpls_to_copy = AUDIO_BUFFER_SIZE < (source->samples - source->position)
                ? AUDIO_BUFFER_SIZE
                : (source->samples - source->position);

            float fadeDirection;
            float fade;

            // start, end fades
            if (source->position < AUDIO_BUFFER_SIZE) {
                fadeDirection = 0.01f;
                fade = 0.f;
            } else if (source->position > source->samples - AUDIO_BUFFER_SIZE) {
                fadeDirection = -0.01f;
                fade = 1.f;
            } else {
                fadeDirection = 0.f;
                fade = 1.f;
            }

            for (unsigned j = 0; j < smpls_to_copy; j++) {

                struct smp {
                    int16_t l;
                    int16_t r;
                } *smp = (struct smp*)(source->data) + (j + source->position);

                e->buffer[j][0] += smp->l * source->vol * e->master_vol;
                e->buffer[j][1] += smp->r * source->vol * e->master_vol;

                fade += fadeDirection;
            }

            source->position += smpls_to_copy;

            if (source->position >= source->samples) {
                e->free_slot = source->id;
                source->position = 0;
                source->state = STATE_READY;
                source->id = 0;
            }
        }
    }

    //Send the prepared audio buffer
    (*(snd_queue))->Enqueue(snd_queue, e->buffer, sizeof(int16_t[AUDIO_NUM_CHANNELS]) * AUDIO_BUFFER_SIZE);
}

int audio_init(struct audio* engine) {

    //Initializing the sound sources
    //These require more than the trivial initializer because of function overloading.

    memset(engine, 0, sizeof(struct audio));
    engine->master_vol = 1.f;

    LOGI("Audio Engine init started\n");

    //=================================== Creating the SL Sound Engine ======================================
    const SLuint32 eng_mix_iid_count = 1;
    const SLInterfaceID eng_mix_iids[] = {SL_IID_ENGINE};
    const SLboolean eng_mix_reqs[] = {SL_BOOLEAN_TRUE};

    SLresult result;
    result = slCreateEngine(&engine->sl_engine, 0, NULL, eng_mix_iid_count, eng_mix_iids, eng_mix_reqs);
    if(result != SL_RESULT_SUCCESS)
    {
        LOGE("slCreateEngine failed\n");
        return 0;
    }

    result = (*engine->sl_engine)->Realize(engine->sl_engine, SL_BOOLEAN_FALSE);
    if(result != SL_RESULT_SUCCESS)
    {
        LOGE("engine->sl_engine Realize failed\n");
        return 0;
    }

    result = (*engine->sl_engine)->GetInterface(engine->sl_engine, SL_IID_ENGINE, &engine->sl_engine_interface);
    if(result != SL_RESULT_SUCCESS)
    {
        LOGE("engine->sl_engine GetInterface failed\n");
        return 0;
    }
    //======================================================================================================

    //======================================= Creating the Output Mix object ===============================

    const SLuint32 out_mix_iid_count = 0;
    const SLInterfaceID out_mix_iid[] = {};
    const SLboolean out_mix_req[] = {};

    result = (*engine->sl_engine_interface)->CreateOutputMix(engine->sl_engine_interface, &engine->sl_output_mix, out_mix_iid_count, out_mix_iid,
                                                             out_mix_req);
    if(result != SL_RESULT_SUCCESS)
    {
        LOGE("engine->sl_engine CreatOutputMix failed\n");
        return 0;
    }
    result = (*engine->sl_output_mix)->Realize(engine->sl_output_mix, SL_BOOLEAN_FALSE);
    if(result != SL_RESULT_SUCCESS)
    {
        LOGE("engine->sl_engine output_mix_object Realize failed\n");
        return 0;
    }

    //======================================================================================================

    //============================================= Setting up Audio Player ================================

    //============================ Setting up data sources =================================================

    SLDataLocator_AndroidSimpleBufferQueue bufq_loc;
    bufq_loc.locatorType = SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE;
    bufq_loc.numBuffers = AUDIO_NUM_CHANNELS;

    //Format of the audio data
    SLDataFormat_PCM format_pcm;
    format_pcm.formatType = SL_DATAFORMAT_PCM;
    format_pcm.numChannels = AUDIO_NUM_CHANNELS;//1 for mono audio, 2 for stereo audio
    format_pcm.samplesPerSec = SL_SAMPLINGRATE_48;
    format_pcm.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_16;
    format_pcm.containerSize = SL_PCMSAMPLEFORMAT_FIXED_16;
    format_pcm.channelMask = SL_SPEAKER_FRONT_RIGHT | SL_SPEAKER_FRONT_LEFT;//SL_SPEAKER_FRONT_CENTER for mono audio
    format_pcm.endianness = SL_BYTEORDER_LITTLEENDIAN;

    //Setting up the audio data source input
    SLDataSource audio_source = {&bufq_loc, &format_pcm};
    audio_source.pLocator = &bufq_loc;
    audio_source.pFormat = &format_pcm;

    //Setting up the audio data source output
    SLDataLocator_OutputMix data_locator_out;
    data_locator_out.locatorType = SL_DATALOCATOR_OUTPUTMIX;
    data_locator_out.outputMix = engine->sl_output_mix;

    SLDataSink data_sink;
    data_sink.pLocator = &data_locator_out;
    data_sink.pFormat = NULL;
    //=================================================================================================

    //================================== Creating the Sound player ====================================
    const SLuint32 snd_plyr_iid_count = 3;
    const SLInterfaceID snd_plyr_iids[] = {SL_IID_PLAY, SL_IID_BUFFERQUEUE, SL_IID_VOLUME};
    const SLboolean snd_plyr_reqs[] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};


    result = (*engine->sl_engine_interface)->CreateAudioPlayer(engine->sl_engine_interface, &engine->sl_audio_player, &audio_source, &data_sink,
                                                               snd_plyr_iid_count, snd_plyr_iids, snd_plyr_reqs);
    if(result != SL_RESULT_SUCCESS)
    {
        LOGE("engine->sl_engine CreateAudioPlayer failed\n");
        return 0;
    }

    result = (*engine->sl_audio_player)->Realize(engine->sl_audio_player, SL_BOOLEAN_FALSE);
    if(result != SL_RESULT_SUCCESS)
    {
        LOGE("sl_audio_player Realize failed\n");
    }

    //Getting the three interfaces we requested above
    result = (*engine->sl_audio_player)->GetInterface(engine->sl_audio_player, SL_IID_PLAY, &engine->sl_audio_player_interface);
    if(result != SL_RESULT_SUCCESS)
    {
        LOGE("sl_audio_player GetInterface resume failed\n");
        return 0;
    }

    result = (*engine->sl_audio_player)->GetInterface(engine->sl_audio_player, SL_IID_BUFFERQUEUE, &engine->sl_buffer_queue_interface);
    if(result != SL_RESULT_SUCCESS)
    {
        LOGE("sl_audio_player GetInterface buffer queue failed\n");
        return 0;
    }

    result = (*engine->sl_audio_player)->GetInterface(engine->sl_audio_player, SL_IID_VOLUME, &engine->sl_volume_interface);
    if(result != SL_RESULT_SUCCESS)
    {
        LOGE("sl_audio_player GetInterface volume failed\n");
        return 0;
    }

    //===========================================================================================================
    //=============== Setting the Buffer Swapping Callback ======================================================
    // called when current buffer is done playing to prepare the next buffer
    result =
        (*engine->sl_buffer_queue_interface)->RegisterCallback(engine->sl_buffer_queue_interface, sl_buffer_callback, engine);//(void*)&oengine->sl_engine

    if(result != SL_RESULT_SUCCESS)
    {
        LOGE("sl_buffer_queue_interface RegisterCallback failed\n");
        return 0;
    }
    //===========================================================================================================

    memset(engine->buffer, 0, sizeof(engine->buffer));

    (*engine->sl_buffer_queue_interface)->Enqueue(engine->sl_buffer_queue_interface, engine->buffer, sizeof(int16_t[AUDIO_NUM_CHANNELS]) * AUDIO_BUFFER_SIZE);
    engine->state = STATE_READY;

    audio_play_all(engine);
    LOGI("Audio Engine init finished\n");
    return 1;
}


void audio_play_all(struct audio* engine) {
    (*engine->sl_audio_player_interface)->SetPlayState(engine->sl_audio_player_interface, SL_PLAYSTATE_PLAYING);
}

void audio_pause_all(struct audio* engine) {
    (*engine->sl_audio_player_interface)->SetPlayState(engine->sl_audio_player_interface, SL_PLAYSTATE_PAUSED);
}

void audio_stop_all(struct audio* engine) {
    (*engine->sl_audio_player_interface)->SetPlayState(engine->sl_audio_player_interface, SL_PLAYSTATE_STOPPED); // TODO sigsegv
}

void audio_destroy(struct audio* engine) {

    audio_stop_all(engine);

    if(engine->sl_audio_player != NULL)
    {
        SLuint32 sound_player_state;
        (*engine->sl_audio_player)->GetState(engine->sl_audio_player, &sound_player_state);

        if(sound_player_state == SL_OBJECT_STATE_REALIZED)
        {
            (*engine->sl_buffer_queue_interface)->Clear(engine->sl_buffer_queue_interface);
            (*engine->sl_audio_player)->AbortAsyncOperation(engine->sl_audio_player);
            (*engine->sl_audio_player)->Destroy(engine->sl_audio_player);
            engine->sl_audio_player = NULL;
            engine->sl_audio_player_interface = NULL;
            engine->sl_buffer_queue_interface = NULL;
            engine->sl_volume_interface = NULL;
        }
    }

    // Destroying the output mix object
    if(engine->sl_output_mix != NULL)
    {
        (*engine->sl_output_mix)->Destroy(engine->sl_output_mix);
        engine->sl_output_mix = NULL;
    }

    // Destroying the sound engine
    if(engine->sl_engine != NULL)
    {
        (*engine->sl_engine)->Destroy(engine->sl_engine);
        engine->sl_engine = NULL;
        engine->sl_engine_interface = NULL;
    }

    engine->state = STATE_OFF;
}

#else

#include "soundio.h"

#if defined (WIN32) || defined (_WIN32)
#include <Windows.h>
#elif defined (__unix__) || defined (__unix)
#include <pthread.h>
#endif

static void write_callback(struct SoundIoOutStream* outstream, int frame_count_min, int frame_count_max)
{
    const struct SoundIoChannelLayout* layout = &outstream->layout;
    float float_sample_rate = outstream->sample_rate;
    float seconds_per_frame = 1.0f / float_sample_rate;
    struct SoundIoChannelArea* areas;

    struct audio* e = outstream->userdata;

    if (e->state == STATE_OFF)
        return;

	memset(e->buffer, 0, sizeof(e->buffer));

	for (int i = 0; i < AUDIO_MAX_SOURCES; i++) {

		struct audio_source* source = &e->sources[i];

		if (source->state != STATE_BUSY)
			continue;

		int smpls_to_copy = frame_count_max < (source->samples - source->position)
			? frame_count_max
			: (source->samples - source->position);

		if (source->position >= source->samples) {
			e->free_slot = source->id;
			source->position = 0;
			source->state = STATE_READY;
			source->id = 0;
            continue;
		}

		//float fadeDirection;
		//float fade;

		// start, end fades
		//if (source->position < frame_count_max) {
		//	fadeDirection = 0.01f;
		//	fade = 0.f;
		//} else if (source->position > source->samples - frame_count_max) {
		//	fadeDirection = -0.01f;
		//	fade = 1.f;
		//} else {
		//	fadeDirection = 0.f;
		//	fade = 1.f;
		//}

		int err;

		if ((err = soundio_outstream_begin_write(outstream, &areas, &smpls_to_copy))) {
            LOGE("%s\n", soundio_strerror(err));
			exit(1);
		}

		for (int frame = 0; frame < smpls_to_copy; ++frame) {

            int16_t* smp = &source->data[frame + source->position * sizeof(int16_t)];

			for (int channel = 0; channel < layout->channel_count; ++channel) {
				float* ptr = (float*)(areas[channel].ptr + areas[channel].step * frame);
                *ptr += NORMALIZE_INT16(smp[channel]) * source->vol * e->master_vol;
			}

			//fade += fadeDirection;
		}

		if ((err = soundio_outstream_end_write(outstream))) {
            LOGE("%s\n", soundio_strerror(err));
			exit(1);
		}

		source->position += smpls_to_copy;
	}
}

static unsigned long loop_for_events(void* soundio) {
    for (;;) soundio_wait_events((struct SoundIo*)soundio);
}

int audio_init(struct audio* engine) {

    memset(engine, 0, sizeof(struct audio));
    engine->master_vol = 1.f;

    int err;
    struct SoundIo* soundio = soundio_create();
    
    if (!soundio) {
        LOGE("Audio: out of memory\n");
        return 0;
    }

    if ((err = soundio_connect(soundio))) {
        LOGE("Audio: error connecting: %s\n", soundio_strerror(err));
        return 0;
    }

    soundio_flush_events(soundio);

    int default_out_device_index = soundio_default_output_device_index(soundio);
    if (default_out_device_index < 0) {
        LOGE("Audio: no output device found\n");
        return 0;
    }

    struct SoundIoDevice* device = soundio_get_output_device(soundio, default_out_device_index);
    if (!device) {
        LOGE("Audio: out of memory\n");
        return 0;
    }

    LOGI("Audio: output device: %s\n", device->name);

    struct SoundIoOutStream* outstream = soundio_outstream_create(device);
    outstream->format = SoundIoFormatFloat32NE;
    outstream->write_callback = write_callback;
    outstream->userdata = (void*)engine;

    if ((err = soundio_outstream_open(outstream))) {
        LOGE("Audio: unable to open device: %s\n", soundio_strerror(err));
        return 0;
    }

    if (outstream->layout_error)
        LOGE("Audio: unable to set channel layout: %s\n", soundio_strerror(outstream->layout_error));

    if ((err = soundio_outstream_start(outstream))) {
        LOGE("Audio: unable to start device: %s\n", soundio_strerror(err));
        return 0;
    }

#if defined (WIN32) || defined (_WIN32)

    CreateThread(NULL, 0, loop_for_events, soundio, 0, NULL);

#elif defined (__unix__) || defined (__unix)
    pthread_t thread;
    if (pthread_create(&thread, NULL, loop_for_events, (void*) soundio))
        LOGE("Thread: cannot create thread.\n");
#endif

    engine->soundio = (void*)soundio;
    engine->device = (void*)device;
    engine->outstream = (void*)outstream;
    engine->state = STATE_READY;
    audio_play_all(engine);
    LOGI("Audio: engine initialization finished.\n");

    return 1;
}

void audio_play_all(struct audio* engine) {

}

void audio_pause_all(struct audio* engine) {

}

void audio_stop_all(struct audio* engine) {

}

void audio_destroy(struct audio* engine) {

	audio_stop_all(engine);

    soundio_outstream_destroy((struct SoundIoOutStream*)engine->outstream);
    soundio_device_unref((struct SoundIoDevice*)engine->device);
    soundio_destroy((struct SoundIo*)engine->soundio);

    engine->state = STATE_OFF;
}

#endif

void audio_play(struct audio* engine, struct audio_source* source) {

    if (!source) {
        LOGW("Warning: couldn't resume sound, no free sound sources\n");
        return;
    } else if (source->data == NULL) {
        LOGW("Warning: tried playing sound with an uninitialized sample (Sample has null data)\n");
        return;
    } else if (engine->free_slot > AUDIO_MAX_SOURCES) {
        return;
    }

    source->id = engine->free_slot;
    source->state = STATE_BUSY;
    source->position = 0;
    engine->sources[engine->free_slot++] = *source;
}

void audio_pause(struct audio* engine, struct audio_source* source) {
    LOGI("audio_pause is not implemented\n");
}

void audio_stop(struct audio* engine, struct audio_source* source) {
    LOGI("audio_stop is not implemented\n");
}

static void audio_wav_i16i_pcm_read(struct audio_source* source, const char* path) {

    if (source == NULL || !path)
        return;

    struct wav_header {
        /* RIFF Chunk Descriptor */
        uint8_t         RIFF[4];        // RIFF Header Magic header
        uint32_t        ChunkSize;      // RIFF Chunk Size
        uint8_t         WAVE[4];        // WAVE Header
        /* "fmt" sub-chunk */
        uint8_t         fmt[4];         // FMT header
        uint32_t        Subchunk1Size;  // Size of the fmt chunk
        uint16_t        AudioFormat;    // Audio format 1=PCM,6=mulaw,7=alaw,     257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM
        uint16_t        NumOfChan;      // Number of channels 1=Mono 2=Stereo
        uint32_t        SamplesPerSec;  // Sampling Frequency in Hz
        uint32_t        bytesPerSec;    // bytes per second
        uint16_t        blockAlign;     // 2=16-bit mono, 4=16-bit stereo
        uint16_t        bitsPerSample;  // Number of bits per sample
        /* "data" sub-chunk */
        uint8_t         Subchunk2ID[4]; // "data"  string
        uint32_t        Subchunk2Size;  // Sampled data length
    } wav_hdr;

    file_load_asset(&source->wav_file, path);
    if (source->wav_file.size < 1) {
        LOGE("Could not load file %s\n", path);
        return;
    }

    size_t current_byte = 0;
    memcpy(&wav_hdr, source->wav_file.data, current_byte += sizeof(wav_hdr));

    source->size = wav_hdr.Subchunk2Size;

    if (strncasecmp((char*)wav_hdr.Subchunk2ID, "data", 4) != 0) {

        current_byte += wav_hdr.Subchunk2Size + 4; // points to data size

        source->size = *(uint32_t*)((uint8_t*)source->wav_file.data + current_byte);
        current_byte += 4; // points to the data
    }

    source->samples = source->size / wav_hdr.NumOfChan / wav_hdr.bitsPerSample * 8;

    // validate
    if (wav_hdr.bitsPerSample != AUDIO_BITS_PER_SAMPLE) {
        LOGE("Unexpected bitrate\n");
        return;
    } else if (wav_hdr.SamplesPerSec != AUDIO_SAMPLE_RATE) {
        LOGE("Unexpected sample rate\n");
        return;
    } else if (wav_hdr.AudioFormat != 1) {
        LOGE("Audio format %d is not supported\n", wav_hdr.AudioFormat);
        return;
    }

    source->data = (uint16_t*)((uint8_t*)source->wav_file.data + current_byte);
}

int audio_source_load(struct audio_source* source, const char* path, float vol) {

    memset(source, 0, sizeof(struct audio_source));
    source->vol = vol;

    audio_wav_i16i_pcm_read(source, path);

    if (source->size > 0) {
        source->state = STATE_READY;
    } else {
        return 0;
    }

    return 1;
}

void audio_source_unload(struct audio_source* source) {

    if (source->size < 1 || source->data == NULL)
        return;

    file_unload(&source->wav_file);
    source->state = STATE_OFF;
}
