#include "Locator.h"

void real::Locator::RegisterAudioSystem(Audio* audio)
{
	if (audio == nullptr)
		m_AudioSystem.reset(&m_DefaultAudioSystem);
	else
		m_AudioSystem.reset(audio);
}

void real::Locator::ShutDown()
{
	m_AudioSystem.reset(&m_DefaultAudioSystem);
}
