#ifndef LOGAUDIO_H
#define LOGAUDIO_H

#include <memory>

#include "Audio.h"

namespace real
{
	class LogAudio final : public Audio
	{
	public:
		explicit LogAudio(Audio* audio);
		virtual ~LogAudio() override = default;

		LogAudio(const LogAudio& other) = delete;
		LogAudio& operator=(const LogAudio& rhs) = delete;
		LogAudio(LogAudio&& other) = delete;
		LogAudio& operator=(LogAudio&& rhs) = delete;

		virtual void Update() override;
		virtual void Play(const Sound sound, const int volume, const int loops) override;
		virtual void Stop(const Sound sound) override;
		virtual void Stop(const int channel) override;
		virtual void StopAllSounds() override;
		virtual void Mute(bool mute) override;

	private:
		std::unique_ptr<Audio> m_RealAudio;
	};
}

#endif // LOGAUDIO_H