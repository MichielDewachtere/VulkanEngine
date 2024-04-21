#include "InputMap.h"

#include <SDL_scancode.h>

real::InputMap::InputMap(std::string name)
	: m_Name(std::move(name))
{
}

// TODO: Remove by passing game object?
void real::InputMap::RemoveKeyboardAction(uint8_t id)
{
	m_KeyboardActionsToRemove.push_back(id);
}

void real::InputMap::RemoveGamePadAction(uint8_t id, uint8_t controllerId)
{
	m_GamePadActionsToRemove[controllerId].push_back(id);
}

const std::map<uint8_t, std::unique_ptr<real::KeyboardAction>>& real::InputMap::GetKeyboardActions()
{
	if (m_KeyboardActionsToRemove.empty() == false)
	{
		for (const auto& id : m_KeyboardActionsToRemove)
		{
			m_pKeyboardActions.erase(id);
		}

		m_KeyboardActionsToRemove.clear();
	}

	return m_pKeyboardActions;
}

const std::map<uint8_t, std::map<uint8_t, std::unique_ptr<real::ControllerAction>>>& 
	real::InputMap::GetGamePadActions()
{
	if (m_GamePadActionsToRemove.empty() == false)
	{
		for (const auto& [controllerId, indices] : m_GamePadActionsToRemove)
		{
			for (uint8_t index : indices)
			{
				m_pControllerActions.at(controllerId).erase(index);
			}
		}
		m_GamePadActionsToRemove.clear();
	}

	return m_pControllerActions;
}

bool real::InputMap::IsKeyCodeValid(int code)
{
	constexpr int minKeyBoardCode = 0;
	constexpr int maxKeyBoardCode = SDL_NUM_SCANCODES;

	return code > minKeyBoardCode && code < maxKeyBoardCode;
}
