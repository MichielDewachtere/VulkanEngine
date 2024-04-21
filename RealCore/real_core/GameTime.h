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

#endif // REALGAMETIME_H