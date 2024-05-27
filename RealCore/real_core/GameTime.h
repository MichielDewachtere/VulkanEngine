#ifndef REALGAMETIME_H
#define REALGAMETIME_H

#include "Singleton.h"

#include <chrono>

namespace real
{
	class GameTime final : public Singleton<GameTime>
	{
	public:
		explicit GameTime() = default;
		virtual ~GameTime() override = default;

		GameTime(const GameTime& other) = delete;
		GameTime& operator=(const GameTime& rhs) = delete;
		GameTime(GameTime&& other) = delete;
		GameTime& operator=(GameTime&& rhs) = delete;

		virtual void Init() override;
		virtual void Update() override;

		float GetElapsed() const;
		float GetTotal() const;
		uint32_t GetFPS_Unsigned() const { return m_FPS; };
		float GetFPS_Float() const { return m_fFPS; }

		[[nodiscard]] uint32_t StartTimer();

		template<typename DurationType = std::chrono::milliseconds>
		float GetTime(uint32_t id);
		template<typename DurationType = std::chrono::milliseconds>
		float EndTimer(uint32_t id);

	private:
		std::chrono::steady_clock::time_point m_PrevTime{};
		float m_DeltaTime{};
		float m_TotalTime{};

		uint32_t m_FPS = 0;
		float m_fFPS = 0.0f;
		uint32_t m_FPSCount = 0;
		float m_FPSTimer = 0.0f;

		std::vector<std::chrono::steady_clock::time_point> m_Timers{};

	};
}

template <typename DurationType>
float real::GameTime::GetTime(uint32_t id)
{
	const auto endTime = std::chrono::high_resolution_clock::now();
	const auto duration = endTime - m_Timers[id];

	const float time = static_cast<float>(std::chrono::duration_cast<DurationType>(duration).count());
	return time;
}

template<typename DurationType>
float real::GameTime::EndTimer(uint32_t id)
{
	const auto time = GetTime<DurationType>(id);

	m_Timers.erase(m_Timers.begin() + id);

	return static_cast<float>(time);
}


#endif // REALGAMETIME_H