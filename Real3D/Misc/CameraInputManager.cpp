#include "CameraInputManager.h"

#include <SDL2/SDL.h>

void CameraInputManager::Initialize(const real::GameContext& context)
{
	//m_Context = context;

	m_pOldKeyboardState = new byte[SDL_NUM_SCANCODES];
	m_pCurrentKeyboardState = new byte[SDL_NUM_SCANCODES];

	m_OldMouseButtonState = SDL_GetMouseState(nullptr,  nullptr);
	m_CurrentMouseButtonState = m_OldMouseButtonState;
}

void CameraInputManager::CleanUp() const
{
	delete[] m_pOldKeyboardState;
	delete[] m_pCurrentKeyboardState;
}

void CameraInputManager::UpdateInputStates(const real::GameContext& context, bool /*override*/)
{
	//m_IsEnabled = !override;
	//if (m_IsEnabled == false)
	//{
	//	m_MouseMovement = { 0,0 };
	//	return;
	//}

	UpdateKeyboardStates();
	UpdateMouseStates(context);
}

const glm::ivec2& CameraInputManager::GetMouseMovement() const
{
	return m_MouseMovement;
}

bool CameraInputManager::IsKeyboardKey(InputState state, int key) const
{
	if (!IsKeyCodeValid(key))
		return false;

	switch (state)
	{
	case InputState::down:
		return IsKeyboardKeyDown(key, true) && IsKeyboardKeyDown(key, false);
	case InputState::pressed:
		return IsKeyboardKeyDown(key, true) == false && IsKeyboardKeyDown(key, false);
	case InputState::released:
		return IsKeyboardKeyDown(key, true) && IsKeyboardKeyDown(key, false) == false;
	}

	return false;

}

bool CameraInputManager::IsMouseButton(InputState state, MouseButton button) const
{
	switch (state)
	{
	case InputState::down:
		return IsMouseButtonDown(button, true) && IsMouseButtonDown(button, false);
	case InputState::pressed:
		return IsMouseButtonDown(button, true) == false && IsMouseButtonDown(button, false);
	case InputState::released:
		return IsMouseButtonDown(button, true) && IsMouseButtonDown(button, false) == false;
	}

	return false;
}

void CameraInputManager::UpdateKeyboardStates() const
{
	std::copy(m_pCurrentKeyboardState, m_pCurrentKeyboardState + SDL_NUM_SCANCODES, m_pOldKeyboardState);
	const Uint8* currentState = SDL_GetKeyboardState(nullptr);
	std::copy(currentState, currentState + SDL_NUM_SCANCODES, m_pCurrentKeyboardState);
}

void CameraInputManager::UpdateMouseStates(const real::GameContext& context)
{
	m_OldMousePosition = m_CurrentMousePosition;
	m_OldMouseButtonState = m_CurrentMouseButtonState;

	// SDL_GetMouseState gets the current mouse position and button states
	int mouseX, mouseY;
	m_CurrentMouseButtonState = SDL_GetMouseState(&mouseX, &mouseY);

	m_CurrentMousePosition.x = mouseX;
	m_CurrentMousePosition.y = mouseY;

	// Check if mouse pos is in window
	SDL_Point clientPos;
	clientPos.x = mouseX;
	clientPos.y = mouseY;

	const auto rect = SDL_GetWindowMouseRect(context.pWindow);
	if (rect != nullptr && SDL_PointInRect(&clientPos, rect) == SDL_FALSE)
	{
		m_CurrentMousePosition = m_OldMousePosition;
		m_CurrentMouseButtonState = m_OldMouseButtonState;
	}

	m_MouseMovement.x = m_CurrentMousePosition.x - m_OldMousePosition.x;
	m_MouseMovement.y = m_CurrentMousePosition.y - m_OldMousePosition.y;
}

bool CameraInputManager::IsKeyboardKeyDown(int key, bool previousFrame) const
{
	if (previousFrame)
		return m_pOldKeyboardState[key];

	return m_pCurrentKeyboardState[key];
}

bool CameraInputManager::IsMouseButtonDown(MouseButton button, bool previousFrame) const
{
	if (previousFrame)
		return m_OldMouseButtonState & SDL_BUTTON(static_cast<int>(button));

	return m_CurrentMouseButtonState & SDL_BUTTON(static_cast<int>(button));
}

bool CameraInputManager::IsKeyCodeValid(int code)
{
	return (code >= 0 && code <= SDL_NUM_SCANCODES);
}