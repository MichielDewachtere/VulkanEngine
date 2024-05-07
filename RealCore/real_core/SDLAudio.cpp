#include "SDLAudio.h"

#include <deque>
#include <mutex>
#include <ranges>

#include <SDL_mixer.h>

#include "Logger.h"

namespace real
{
	// TODO: Add option to pre load certain audio files?
	class SDLAudio::SDLAudioImpl final : public Audio
	{
	public:
		explicit SDLAudioImpl()
		{
			if (Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG) < 0)
			{
				printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
			}

			if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_CHANNELS, 2048) < 0)
			{
				printf("SDL_mixer could not open audio! SDL_mixer Error: %s\n", Mix_GetError());
			}

			m_SoundsToPlay.resize(max_pending);

			m_PlayingThread = std::jthread(&SDLAudioImpl::PlaySounds, this);
		}
		virtual ~SDLAudioImpl() override
		{
			m_KeepWorking = false;
		}
		SDLAudioImpl(const SDLAudioImpl& other) = delete;
		SDLAudioImpl operator=(const SDLAudioImpl& rhs) = delete;
		SDLAudioImpl(SDLAudioImpl&& other) = delete;
		SDLAudioImpl operator=(SDLAudioImpl&& rhs) = delete;

		void Update() override;
		void Play(const Sound sound, const int volume, const int loops) override;
		void Stop(const Sound sound) override;
		void Stop(const int channel) override;
		void StopAllSounds() override;
		void Mute(bool mute) override;

	private:
		bool m_IsMuted{ false };
		std::map<int, uint8_t> m_ChannelVolumes;
		std::mutex m_ChannelVolumesMutex{};

		static constexpr size_t max_pending = 8;

		std::atomic_bool m_KeepWorking{ true };
		std::mutex m_SoundMutex{};
		std::jthread m_PlayingThread{};
		std::deque<Sound> m_SoundsToPlay;

		void PlaySounds();
	};

	void SDLAudio::SDLAudioImpl::Update() {}

	void SDLAudio::SDLAudioImpl::Play(const Sound sound, const int volume, const int loops)
	{
		// TODO: Merge adjacent sounds with the same id? Or all sounds with the same id in queue?

		std::lock_guard lock(m_SoundMutex);

		if (m_SoundsToPlay.size() > max_pending)
			return;

		m_SoundsToPlay.push_back(sound);

		if (volume != -1)
			m_SoundsToPlay.back().volume = volume;

		if (loops != -1)
			m_SoundsToPlay.back().loops = loops;
	}

	void SDLAudio::SDLAudioImpl::Stop(const Sound sound)
	{
		Mix_HaltChannel(sound.channel);
	}

	void SDLAudio::SDLAudioImpl::Stop(const int channel)
	{
		Mix_HaltChannel(channel);
	}

	void SDLAudio::SDLAudioImpl::StopAllSounds()
	{
		Mix_HaltChannel(-1);
	}

	void SDLAudio::SDLAudioImpl::Mute(bool mute)
	{
		m_IsMuted = mute;

		const int numChannels = Mix_AllocateChannels(-1);

		for (int i = 0; i < numChannels; ++i)
		{
			if (mute)
			{
				Mix_Volume(i, 0);
			}
			else
			{
				std::lock_guard lock(m_ChannelVolumesMutex);
				Mix_Volume(i, m_ChannelVolumes[i]);
			}
		}
	}

	void SDLAudio::SDLAudioImpl::PlaySounds()
	{
		while (m_KeepWorking)
		{
			while (m_SoundsToPlay.empty() == false)
			{
				Sound sound;
				{
					std::lock_guard lock(m_SoundMutex);
					sound = m_SoundsToPlay.front();
					m_SoundsToPlay.pop_front();
				}

				const auto mixChunk = Mix_LoadWAV(sound.fileName.c_str());

				if (!mixChunk)
				{
					Logger::LogError({ "SDL_mixer could not load sound effect file! SDL_mixer Error: {}" }, Mix_GetError());
					break;
				}

				const int result = Mix_PlayChannel(sound.channel, mixChunk, sound.loops);
				if (result == -1)
				{
					Logger::LogError({ "SDL_mixer could not play sound effect! SDL_mixer Error: {}" }, Mix_GetError());
					break;
				}

				if (m_IsMuted)
					Mix_Volume(result, 0);
				else
					Mix_Volume(result, sound.volume);

				//Mix_FreeChunk(mixChunk);
			}
		}
	}
}


real::SDLAudio::SDLAudio()
{
	m_pImpl = std::make_unique<SDLAudioImpl>();
}

void real::SDLAudio::Update()
{
	m_pImpl->Update();
}

void real::SDLAudio::Play(const Sound sound, const int volume, const int loops)
{
	m_pImpl->Play(sound, volume, loops);
}

void real::SDLAudio::Stop(const Sound sound)
{
	m_pImpl->Stop(sound);
}

void real::SDLAudio::StopAllSounds()
{
	m_pImpl->StopAllSounds();
}

void real::SDLAudio::Mute(bool mute)
{
	m_pImpl->Mute(mute);
}

void real::SDLAudio::Stop(const int channel)
{
	m_pImpl->Stop(channel);
}