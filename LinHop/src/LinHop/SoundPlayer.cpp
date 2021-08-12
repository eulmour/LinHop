#include "SoundPlayer.h"

#define PA_CALL(func)\
    {\
    PaError err = func;\
    if (err != paNoError) (printf(#func ": %s\n", Pa_GetErrorText(err)));\
    }\
    

void InitAudioPlayer()
{
    PaError err = Pa_Initialize();
    if (err != paNoError) printf("Pa_Initialize: %d\n", err);

#ifdef _DEBUG
    printf("Initialising PortAudio. Version: %d\n", Pa_GetVersion());
    puts("----------------------\n");
    puts("Devices:\n");

    int numDevices = Pa_GetDeviceCount();

    for (int i = 0; i < numDevices; i++)
    {
        auto deviceInfo = Pa_GetDeviceInfo(i);
        puts("----------------------");
        printf("Name: %s\n", deviceInfo->name);
        printf("HostApi: %d\n", deviceInfo->hostApi);
        printf("SampleRate: %lf\n", deviceInfo->defaultSampleRate);
        printf("InputChannels: %d\n", deviceInfo->maxInputChannels);
        printf("OutputChannels: %d\n", deviceInfo->maxOutputChannels);
        puts("----------------------\n");
    }
#endif
}

void DestroyAudioPlayer()
{
    PaError err = Pa_Terminate();
    if (err != paNoError) printf("Pa_Terminate: %d\n", err);
}


static int patestCallback(
    const void* inputBuffer,
    void* outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void* userData)
{
    // Prevent warnings
    (void)inputBuffer;
    (void)timeInfo;
    (void)statusFlags;

    // an AudioFile gets passed as userData
    AudioFile* file = reinterpret_cast<AudioFile*>(userData);
    float* out = static_cast<float*>(outputBuffer);

    sf_seek(file->file, file->readHead, SF_SEEK_SET);

    float* data = new float[framesPerBuffer * file->info.channels];

    file->count = sf_read_float(
        file->file,
        data,
        framesPerBuffer * file->info.channels);

    for (int i = 0; i < framesPerBuffer * file->info.channels; ++i)
    {
        *out++ = data[i] * file->volume;
    }

    file->readHead += file->buffer_size;

    delete[] data;

    if (file->count > 0) return paContinue;
    else return paComplete;
}

AudioFile::AudioFile(const char* path)
{
    ::memset(&info, 0, sizeof(info));
    file = sf_open(path, SFM_READ, &info);

    PaStreamParameters params;
    params.device = Pa_GetDefaultOutputDevice();
    params.channelCount = info.channels;
    params.sampleFormat = paFloat32;
    params.suggestedLatency = Pa_GetDeviceInfo(params.device)->defaultLowOutputLatency;
    params.hostApiSpecificStreamInfo = nullptr;

    PA_CALL(Pa_IsFormatSupported(nullptr, &params, info.samplerate));

    err = Pa_OpenStream(
        &stream,
        nullptr,
        &params,
        info.samplerate,
        buffer_size,
        paClipOff,
        &patestCallback,
        this);

    if (err != paNoError) printf("Pa_OpenStream: %s\n", Pa_GetErrorText(err));
}

AudioFile::~AudioFile()
{
    PA_CALL(Pa_CloseStream(stream));
    sf_close(file);
}

void AudioFile::playFile()
{
    if (this->isPlaying == true)
        StopPlayback();

    StartPlayback();

    //while (file.count > 0) {} /* wait until file finishes playing */

    audioThread = std::thread(
        [this]()
        {
            Pa_Sleep(static_cast<long>(((double)info.frames / (double)info.samplerate) * 999.9)); /* leave before very end*/
            StopPlayback();
        });

    audioThread.detach();
    readHead = 0; /* reset playback position */
}

void AudioFile::StartPlayback()
{
    PA_CALL(Pa_StartStream(stream)); /* stream starts here */
    isPlaying = true;
}

void AudioFile::StopPlayback()
{
    PA_CALL(Pa_StopStream(stream)); /* stream stops here */
    isPlaying = false;
}
