#ifndef ENGINEBASE_H
#define ENGINEBASE_H

#include <functional>
#include <SDL_video.h>
#include <string>

namespace real
{
	class EngineBase
	{
	public:
		explicit EngineBase() = default;
		virtual ~EngineBase();

		EngineBase(const EngineBase& other) = delete;
		EngineBase& operator=(const EngineBase& rhs) = delete;
		EngineBase(EngineBase&& other) = delete;
		EngineBase& operator=(EngineBase&& rhs) = delete;

		virtual void Run(const std::function<void()>& load) = 0;

		static SDL_Window* GetWindow() { return m_pWindow; }


	protected:
		static void InitWindow(const std::string& name, int width, int height, uint32_t flags);

	private:
		static inline SDL_Window* m_pWindow{ nullptr };
	};
}

#endif // ENGINEBASE_H