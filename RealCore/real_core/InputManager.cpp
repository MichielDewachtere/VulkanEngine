#include "InputManager.h"

#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#define NOMINMAX

#include <ranges>
#include <algorithm>

#include <SDL_events.h>
#include <Xinput.h>

#include "EngineBase.h"
#include "imgui_impl_sdl2.h"
#include "Logger.h"
#include "glm/geometric.hpp"

real::InputManager::InputManager() = default;
real::InputManager::~InputManager() = default;

bool real::InputManager::ProcessInput()
{
	SDL_Event e;


	while (SDL_PollEvent(&e))
	{
		ImGui_ImplSDL2_ProcessEvent(&e);
		if (e.type == SDL_QUIT) 
		{
			return false;
		}
	}

	if (m_pActiveInputMap == nullptr)
		return true;

	if (m_GamePadEnabled && m_pGamePads.empty() == false)
	{
		std::ranges::for_each(m_pGamePads, [](const auto& gp)
			{
				gp->Update();
			});

		ProcessGamePadInput();
	}

	if (m_MouseEnabled)
	{
		ProcessMouseInput();
	}

	if (m_KeyboardEnabled)
	{
		ProcessKeyboardInput();
	}

	return true;
}

real::InputMap* real::InputManager::AddInputMap(const std::string& name, bool isActive)
{
	const auto [it, value] = m_pInputMaps.try_emplace(name, std::make_unique<InputMap>(name));
	if (value == false)
	{
		Logger::LogWarning({ "Already an input map with name {}" }, name);
		return (*it).second.get();
	}

	if (isActive)
		m_pActiveInputMap = (*it).second.get();

	return (*it).second.get();

	//m_pInputMaps.push_back(std::make_unique<InputMap>(std::move(name)));
}
void real::InputManager::SetInputMapActive(const std::string& name)
{
	if (m_pInputMaps.contains(name) == false)
	{
		Logger::LogWarning({ "No input map found with name {}" }, name);
		return;
	}

	m_pActiveInputMap = m_pInputMaps[name].get();
}
real::InputMap* real::InputManager::GetInputMap(const std::string& name) const
{
	if (m_pInputMaps.contains(name) == false)
	{
		Logger::LogWarning({ "No input map found with name {}" }, name);
		return nullptr;
	}

	return m_pInputMaps.at(name).get();

}
real::InputMap* real::InputManager::GetActiveInputMap() const
{
	return m_pActiveInputMap;
}

uint8_t real::InputManager::RegisterGamePad()
{
	if (const auto controllers = RegisterGamePadsHelper(true); 
		controllers.empty() == false)
	{
		return controllers.front();
	}

	Logger::LogWarning({ "No available game pad slot found" });
	return UCHAR_MAX;

}
std::vector<uint8_t> real::InputManager::RegisterGamePads()
{
	std::vector<uint8_t> controllers = RegisterGamePadsHelper(false);
	return controllers;
}
real::GamePad* real::InputManager::GetGamePad(unsigned idx) const
{
	const auto it = std::ranges::find_if(m_pGamePads, [idx](const auto& gp)
		{
			return gp->GetIndex() == idx;
		});

	if (it == m_pGamePads.end())
	{
		Logger::LogWarning({ "No game pad found with index {}" }, idx);
		return nullptr;
	}

	return (*it).get();
}
std::vector<real::GamePad*> real::InputManager::GetGamePads() const
{
	std::vector<GamePad*> v{};

	std::ranges::for_each(m_pGamePads, [&v](const auto& gp)
		{
			v.push_back(gp.get());
		});

	return v;
}

void real::InputManager::RemoveGameObjectCommands(const GameObject* pGo)
{
	for (const auto& map : m_pInputMaps | std::views::values)
	{
		map->RemoveAction(pGo);
	}
}

void real::InputManager::UpdateKeyboardStates()
{
	// TODO: Faster than SDL_PollEvent??
	std::ranges::copy(m_pCurrentKeyboardState, m_pOldKeyboardState.begin());
	const Uint8* currentState = SDL_GetKeyboardState(nullptr);
	std::copy(currentState, currentState + SDL_NUM_SCANCODES, m_pCurrentKeyboardState.begin());
}

void real::InputManager::UpdateMouseStates()
{
	m_OldMouseState = m_CurrentMouseState;
	m_OldMousePosition = m_CurrentMousePosition;

	// SDL_GetMouseState gets the current mouse position and button states
	int mouseX, mouseY;
	m_CurrentMouseState = SDL_GetMouseState(&mouseX, &mouseY);

	m_CurrentMousePosition.x = mouseX;
	m_CurrentMousePosition.y = mouseY;

	// Check if mouse pos is in window
	SDL_Point clientPos;
	clientPos.x = mouseX;
	clientPos.y = mouseY;

	const auto rect = SDL_GetWindowMouseRect(EngineBase::GetWindow());
	if (rect != nullptr && SDL_PointInRect(&clientPos, rect) == SDL_FALSE)
	{
		m_CurrentMousePosition = m_OldMousePosition;
		m_CurrentMouseState = m_OldMouseState;
	}

	m_MouseMovement.x = m_CurrentMousePosition.x - m_OldMousePosition.x;
	m_MouseMovement.y = m_CurrentMousePosition.y - m_OldMousePosition.y;

	if (m_MouseMovement == glm::ivec2{ 0,0 })
		m_NormalizedMouseMovement = glm::vec2{ 0,0 };
	else
		m_NormalizedMouseMovement = glm::normalize(glm::vec2(m_MouseMovement));
}

bool real::InputManager::IsKeyboardKeyDown(int key, bool previousFrame) const
{
	if (previousFrame)
		return m_pOldKeyboardState[key];

	return m_pCurrentKeyboardState[key];
}

bool real::InputManager::IsMouseButtonDown(MouseButton button, bool previousFrame) const
{
	if (previousFrame)
		return m_OldMouseState & SDL_BUTTON(static_cast<int>(button));

	return m_CurrentMouseState & SDL_BUTTON(static_cast<int>(button));
}

std::vector<uint8_t> real::InputManager::RegisterGamePadsHelper(bool one)
{
	std::vector<uint8_t> controllers;

	for (uint8_t i{ 0 }; i < m_MaxAmountOfControllers; ++i)
	{
		if (HasGamePad(i))
			continue;

		XINPUT_STATE state;
		const DWORD result = XInputGetState(i, &state);
		if (result == ERROR_SUCCESS)
		{
			m_pGamePads.push_back(std::make_unique<GamePad>(i));
			controllers.push_back(i);

			if (one)
			{
				return controllers;
			}
		}

		if (result == ERROR_DEVICE_NOT_CONNECTED)
		{
			Logger::LogDebug({ "No game pad found with index {}" }, i);
		}
	}

	return controllers;
}

void real::InputManager::ProcessKeyboardInput()
{
	UpdateKeyboardStates();

	for (const auto& action : m_pActiveInputMap->GetKeyboardActions() | std::views::values)
	{
		bool executeCommand = false;

		switch (action->event)
		{
		case KeyState::keyDown:
			executeCommand = !IsKeyboardKeyDown(action->scancode, true) && IsKeyboardKeyDown(action->scancode, false);
			break;
		case KeyState::keyPressed:
			executeCommand = IsKeyboardKeyDown(action->scancode, true) && IsKeyboardKeyDown(action->scancode, false);
			break;
		case KeyState::keyUp:
			executeCommand = IsKeyboardKeyDown(action->scancode, true) && !IsKeyboardKeyDown(action->scancode, false);
			break;
		default:
			break;
		}

		if (executeCommand)
			action->pCommand->Execute();
	}
}

void real::InputManager::ProcessMouseInput()
{
	UpdateMouseStates();

	for (const auto& action : m_pActiveInputMap->GetMouseActions() | std::views::values)
	{
		bool executeCommand = false;

		switch (action->event)
		{
		case KeyState::keyDown:
			executeCommand = !IsMouseButtonDown(action->button, true) && IsMouseButtonDown(action->button, false);
			break;
		case KeyState::keyUp:
			executeCommand = IsMouseButtonDown(action->button, true) && !IsMouseButtonDown(action->button, false);
			break;
		case KeyState::keyPressed:
			executeCommand = IsMouseButtonDown(action->button, true) && IsMouseButtonDown(action->button, false);
			break;
		default:
			break;
		}

		if (executeCommand)
			action->pCommand->Execute();
	}
}

void real::InputManager::ProcessGamePadInput() const
{
	auto executeCommand = [this](const std::unique_ptr<GamePad>& gp)
		{
			if (!m_pActiveInputMap->GetGamePadActions().contains(gp->GetIndex()))
				return;

			for (const auto& action : m_pActiveInputMap->GetGamePadActions().at(gp->GetIndex()) | std::views::values)
			{
				switch (action->inputType)
				{
				case KeyState::keyPressed:
					if (gp->IsPressed(action->button))
						action->pCommand->Execute();
					break;
				case KeyState::keyDown:
					if (gp->IsDown(action->button))
						action->pCommand->Execute();
					break;
				case KeyState::keyUp:
					if (gp->IsUp(action->button))
						action->pCommand->Execute();
					break;
				default:
					break;
				}
			}
		};

	// ReSharper disable once CppExpressionWithoutSideEffects
	std::ranges::for_each(m_pGamePads, executeCommand);
}
bool real::InputManager::HasGamePad(uint8_t index)
{
	const auto it = std::ranges::find_if(m_pGamePads, [index](const auto& pController)
		{
			return index == pController->GetIndex();
		});

	return it != m_pGamePads.end();
}