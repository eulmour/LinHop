#include <thread>
#include <sndfile.hh>
#include <portaudio.h>

class AudioFile
{
public:
	bool isPlaying = false;

	AudioFile(const char* path);
	~AudioFile();
	void playFile();

	SNDFILE*	file = nullptr;
	SF_INFO		info;
	int			buffer_size = 1024;
	int			readHead = 0;
	sf_count_t	count = 2;
	float		volume = 0.5f;
private:
	std::thread audioThread;

	PaStream* stream = nullptr;
	PaError err = 0;

	void StartPlayback();
	void StopPlayback();
};

void InitAudioPlayer();
void DestroyAudioPlayer();

template <typename T, size_t n>
void UpdateVolume(T(&audioFiles)[n], float volume)
{
	for (AudioFile& af : audioFiles)
		af.volume = volume;
}
