#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <map>

#include "Util/Singleton.h"
#include "Util/Structs.h"

#include <glm/glm.hpp>

typedef unsigned char* p_byte;
typedef unsigned char byte;

enum class InputState
{
	pressed,
	released,
	down
};

enum class MouseButton : char
{
	left = 1,
	middle = 2,
	right = 3
};

class InputManager final : public Singleton<InputManager>
{
public:
	~InputManager() override = default;

	InputManager(const InputManager&) = delete;
	InputManager& operator=(const InputManager&) = delete;
	InputManager(InputManager&&) = delete;
	InputManager& operator=(InputManager&&) = delete;

	void Initialize(const GameContext& context);
	void CleanUp() const;

	void UpdateInputStates(const GameContext& context, bool override);

	const glm::ivec2& GetMouseMovement() const;

	bool IsKeyboardKey(InputState state, int key) const;
	bool IsMouseButton(InputState state, MouseButton button) const;

private:
	friend class Singleton<InputManager>;
	InputManager() = default;

	bool m_FirstFrame{ true };
	//GameContext m_Context;
	p_byte m_pCurrentKeyboardState{}, m_pOldKeyboardState{};
	glm::ivec2 m_CurrentMousePosition, m_OldMousePosition, m_MouseMovement;
	uint32_t m_CurrentMouseButtonState{}, m_OldMouseButtonState{};


	void UpdateKeyboardStates() const;
	void UpdateMouseStates(const GameContext& context);

	bool IsKeyboardKeyDown(int key, bool previousFrame = false) const;
	bool IsMouseButtonDown(MouseButton button, bool previousFrame = false) const;

	static bool IsKeyCodeValid(int code);
};

#endif // INPUTMANAGER_H