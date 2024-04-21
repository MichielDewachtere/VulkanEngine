#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <string>
#include <vector>
#include <map>
#include <SDL_events.h>

#include "InputMap.h"
#include "Singleton.h"

namespace real
{
	class GameObject;

	typedef unsigned char* p_byte;

	class InputManager final : public Singleton<InputManager>
	{
	public:
		virtual ~InputManager() override;

		bool ProcessInput();

		InputMap* AddInputMap(const std::string& name, bool isActive = false);
		void SetInputMapActive(const std::string& name);
		InputMap* GetInputMap(const std::string& name) const;
		InputMap* GetActiveInputMap() const;

		uint8_t RegisterGamePad();
		std::vector<uint8_t> RegisterGamePads();
		GamePad* GetGamePad(unsigned int idx) const;
		std::vector<GamePad*> GetGamePads() const;

		void SetMaxAmountOfGamePads(const uint8_t amount) { m_MaxAmountOfControllers = amount; }
		void EnableKeyboardInput(bool enable) { m_KeyboardEnabled = enable; }
		void EnableGamePadInput(bool enable) { m_GamePadEnabled = enable; }

	private:
		friend class Singleton<InputManager>;
		InputManager();

		std::map<std::string, std::unique_ptr<InputMap>> m_pInputMaps;
		//std::vector<std::unique_ptr<InputMap>> m_pInputMaps;
		InputMap* m_pActiveInputMap{ nullptr };
		std::vector<std::unique_ptr<GamePad>> m_pGamePads;
		uint8_t m_MaxAmountOfControllers{ 4 };
		bool m_KeyboardEnabled{ true }, m_GamePadEnabled{ false };
		bool m_KeyboardState0Active{ true };
		p_byte m_pCurrentKeyboardState{ nullptr }, m_pOldKeyboardState{ nullptr };

		std::vector<uint8_t>RegisterGamePadsHelper(bool one);

		void ProcessKeyboardInput() const;
		void ProcessGamePadInput() const;

		void UpdateKeyboardStates() const;
		bool IsKeyboardKeyDown(int key, bool previousFrame = false) const;

		bool HasGamePad(uint8_t index);

		const static inline std::map<KeyState, uint32_t> keyboard_event_to_sdl_event
		{
			{KeyState::keyDown, SDL_KEYDOWN},
			{KeyState::keyUp, SDL_KEYUP},
		};
	};
}

#endif // INPUTMANAGER_H