#include "GameTime.h"

void real::GameTime::Init()
{
	m_PrevTime = std::chrono::high_resolution_clock::now();
}

void real::GameTime::Update()
{
	const auto currentTime = std::chrono::high_resolution_clock::now();

	m_DeltaTime = std::chrono::duration<float>(currentTime - m_PrevTime).count();
	m_TotalTime += m_DeltaTime;

	m_PrevTime = currentTime;

	//FPS LOGIC
	m_FPSTimer += m_DeltaTime;
	++m_FPSCount;
	if (m_FPSTimer >= 1.0f)
	{
		m_fFPS = static_cast<float>(m_FPSCount) / m_FPSTimer;
		m_FPS = m_FPSCount;
		m_FPSCount = 0;
		m_FPSTimer = 0.0f;
	}
}

float real::GameTime::GetElapsed() const
{
	return m_DeltaTime;
}

float real::GameTime::GetTotal() const
{
	return m_TotalTime;
}

uint32_t real::GameTime::StartTimer()
{
	const auto id = m_Timers.size();
	m_Timers.push_back(std::chrono::high_resolution_clock::now());
	return static_cast<uint32_t>(id);
}

float real::GameTime::EndTimer(uint32_t id)
{
	const auto endTime = std::chrono::high_resolution_clock::now();
	const auto duration = endTime - m_Timers[id];

	// Convert duration to milliseconds
	const auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

	// Remove the timer from the vector
	m_Timers.erase(m_Timers.begin() + id);

	return static_cast<float>(milliseconds);
}
