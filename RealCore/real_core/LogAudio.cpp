#include "LogAudio.h"

#include "Logger.h"

real::LogAudio::LogAudio(Audio* audio)
	: m_RealAudio(audio)
{
}

void real::LogAudio::Update()
{
	m_RealAudio->Update();
}

void real::LogAudio::Play(const Sound sound, const int volume, const int loops)
{
	m_RealAudio->Play(sound, volume, loops);
	Logger::LogInfo<>({ "Attempting to play sound {}, attached to file {}" }, sound.id, sound.fileName);
}

void real::LogAudio::Stop(const int channel)
{
	m_RealAudio->Stop(channel);
	Logger::LogInfo({ "Stopped playing channel {}" }, channel);
}

void real::LogAudio::Stop(const Sound sound)
{
	m_RealAudio->Stop(sound);
	Logger::LogInfo({ "Stopped playing sound {}, attached to file {}" }, sound.id, sound.fileName);
}

void real::LogAudio::StopAllSounds()
{
	m_RealAudio->StopAllSounds();
	Logger::LogInfo({ "Stopped all sounds from playing" });
}

void real::LogAudio::Mute(bool mute)
{
	m_RealAudio->Mute(mute);
	if (mute)
		Logger::LogInfo({ "Muted all audio currently playing" });
	else
		Logger::LogInfo({ "Un-muted all audio currently playing" });
}
