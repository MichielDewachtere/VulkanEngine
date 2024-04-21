#ifndef INPUTMAP_H
#define INPUTMAP_H

#include <map>
#include <string>
#include <vector>
#include <stdexcept>

#include "Command.h"
#include "InputTypes.h"

namespace real
{
	class InputMap final
	{
	public:
		explicit InputMap(std::string name);
		~InputMap() = default;

		InputMap(const InputMap& other) = delete;
		InputMap& operator=(const InputMap& rhs) = delete;
		InputMap(InputMap&& other) = delete;
		InputMap& operator=(InputMap&& rhs) = delete;

		template <class CommandType, typename... CommandArgs>
			requires std::is_base_of_v<Command, CommandType>
		void AddKeyboardAction(uint8_t id, KeyState event, uint32_t scancode, CommandArgs... commandArgs);
		template <class CommandType, typename... CommandArgs>
			requires std::is_base_of_v<Command, CommandType>
		void AddGamePadAction(uint8_t controllerId, uint8_t id, KeyState inputType, GamePad::Button button, CommandArgs... commandArgs);

		void RemoveKeyboardAction(uint8_t id);
		void RemoveGamePadAction(uint8_t id, uint8_t controllerId);

		// TODO: Remove commands??

		const std::map<uint8_t, std::unique_ptr<KeyboardAction>>& GetKeyboardActions();
		const std::map<uint8_t, std::map<uint8_t, std::unique_ptr<ControllerAction>>>& GetGamePadActions();

	private:
		std::string m_Name;

		std::map<uint8_t, std::unique_ptr<KeyboardAction>> m_pKeyboardActions{};
		std::map<uint8_t, std::map<uint8_t, std::unique_ptr<ControllerAction>>> m_pControllerActions{};
		std::vector<uint8_t> m_KeyboardActionsToRemove;
		std::map<uint8_t, std::vector<uint8_t>> m_GamePadActionsToRemove;

		static bool IsKeyCodeValid(int code);
	};

	template<class CommandType, typename ...CommandArgs>
		requires std::is_base_of_v<Command, CommandType>
	void InputMap::AddKeyboardAction(uint8_t id, KeyState event, uint32_t scancode, CommandArgs ...commandArgs)
	{
		//if (m_pKeyboardActions.contains(id))
		//	throw std::runtime_error("A keyboard input is already registered with id: {}" + id);

		if (IsKeyCodeValid(static_cast<int>(scancode)) == false)
			throw std::runtime_error(scancode + " is not a valid scancode");

		m_pKeyboardActions[id] = std::make_unique<KeyboardAction>(event, scancode, new CommandType(id, -1, commandArgs...));

		//m_pKeyboardActions.try_emplace(id, std::make_unique<KeyboardAction>(event, scancode, new CommandType(commandArgs...)));
	}

	template <class CommandType, typename ... CommandArgs>
		requires std::is_base_of_v<Command, CommandType>
	void InputMap::AddGamePadAction(uint8_t controllerId, uint8_t id, KeyState inputType,
	                                GamePad::Button button, CommandArgs... commandArgs)
	{
		//if (m_pControllerActions.contains(controllerId))
		//{
		//	Logger::LogWarning({ "A game pad input is already registered with id: {}" }, controllerId);
		//	return;
		//}

		m_pControllerActions[controllerId][id] = std::make_unique<ControllerAction>(controllerId, inputType, button, new CommandType(id, controllerId, commandArgs...));
	}
}

#endif // INPUTMAP_H