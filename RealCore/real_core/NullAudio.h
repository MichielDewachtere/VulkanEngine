#ifndef NULLAUDIO_H
#define NULLAUDIO_H

#include "Audio.h"

namespace real
{
	class NullAudio final : public Audio
	{
	public:
		explicit NullAudio() : Audio() {}
		virtual ~NullAudio() override = default;
		NullAudio(const NullAudio& other) = delete;
		NullAudio operator=(const NullAudio& rhs) = delete;
		NullAudio(NullAudio&& other) = delete;
		NullAudio operator=(NullAudio&& rhs) = delete;

		virtual void Update() override {}
		virtual void Play(const Sound, const int, const int) override {}
		virtual void Stop(const Sound) override {}
		virtual void Stop(const int) override {}
		virtual void StopAllSounds() override {}
		virtual void Mute(bool) override {}
	};
}
#endif // NULLAUDIO_H