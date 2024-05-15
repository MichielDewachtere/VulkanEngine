#include "InputMap.h"

#include <ranges>
#include <SDL_scancode.h>

#include "GameObject.h"

real::InputMap::InputMap(std::string name)
	: m_Name(std::move(name))
{
}

void real::InputMap::RemoveKeyboardAction(uint8_t id)
{
	m_KeyboardActionsToRemove.push_back(id);
}

void real::InputMap::RemoveMouseAction(uint8_t id)
{
	m_MouseActionsToRemove.push_back(id);
}

void real::InputMap::RemoveGamePadAction(uint8_t id, int controllerId)
{
	if (controllerId >= 0)
		m_GamePadActionsToRemove[static_cast<uint8_t>(controllerId)].push_back(id);
	else if (controllerId == -1)
	{
		for (const auto& currentId : m_pControllerActions | std::views::keys)
		{
			m_GamePadActionsToRemove[currentId].push_back(id);
		}
	}
}

void real::InputMap::RemoveAction(const GameObject* pGameObject)
{
	for (const auto& [id, action] : m_pKeyboardActions)
	{
		if (const auto command = dynamic_cast<GameObjectCommand*>(action->pCommand.get());
			command != nullptr)
		{
			if (command->GetGameObject()->GetId() == pGameObject->GetId())
			{
				m_KeyboardActionsToRemove.push_back(id);
			}
		}
	}

	for (const auto& [id, action] : m_pMouseActions)
	{
		if (const auto command = dynamic_cast<GameObjectCommand*>(action->pCommand.get());
			command != nullptr)
		{
			if (command->GetGameObject()->GetId() == pGameObject->GetId())
			{
				m_MouseActionsToRemove.push_back(id);
			}
		}
	}

	for (const auto& [controller, v] : m_pControllerActions)
	{
		for (const auto& [id, action] : v)
		{
			if (const auto command = dynamic_cast<GameObjectCommand*>(action->pCommand.get());
				command != nullptr)
			{
				if (command->GetGameObject()->GetId() == pGameObject->GetId())
				{
					m_GamePadActionsToRemove[controller].push_back(id);
				}
			}
		}
	}
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

const std::map<uint8_t, std::unique_ptr<real::MouseAction>>& real::InputMap::GetMouseActions()
{
	if (m_MouseActionsToRemove.empty() == false)
	{
		for (const auto& id : m_MouseActionsToRemove)
		{
			m_pMouseActions.erase(id);
		}

		m_MouseActionsToRemove.clear();
	}

	return m_pMouseActions;
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
