#ifndef AUDIO_H
#define AUDIO_H

#include <string>

namespace real
{
	using sound_id = unsigned short;

	struct Sound
	{
		sound_id id;
		int volume;
		int loops;
		int channel;
		std::string fileName;

		bool operator<(const Sound& other) const
		{
			return id < other.id;
		}
	};

	class Audio
	{
	public:
		virtual ~Audio() = default;

		virtual void Update() = 0;

		virtual void Play(const Sound sound, const int volume = -1, const int loops = -1) = 0;
		virtual void Stop(const Sound sound) = 0;
		virtual void Stop(const int channel) = 0;
		virtual void StopAllSounds() = 0;
		virtual void Mute(bool mute) = 0;
	};
}

#endif // AUDIO_H