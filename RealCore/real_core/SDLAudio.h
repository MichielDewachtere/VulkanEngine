#ifndef SDLAUDIO_H
#define SDLAUDIO_H

#include <memory>

#include "Audio.h"

namespace real
{
	class SDLAudio final : public Audio
	{
	public:
		explicit SDLAudio();
		virtual ~SDLAudio() override = default;
		SDLAudio(const SDLAudio& other) = delete;
		SDLAudio operator=(const SDLAudio& rhs) = delete;
		SDLAudio(SDLAudio&& other) = delete;
		SDLAudio operator=(SDLAudio&& rhs) = delete;

		void Update() override;

		virtual void Play(const Sound sound, const int volume = -1, const int loops = -1) override;
		virtual void Stop(const Sound sound) override;
		virtual void StopAllSounds() override;
		virtual void Mute(bool mute) override;
		virtual void Stop(const int channel) override;

	private:
		class SDLAudioImpl;
		std::unique_ptr<SDLAudioImpl> m_pImpl;
	};
}

#endif // SDLAUDIO_H