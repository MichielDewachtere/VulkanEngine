#ifndef INPUTTYPES_H
#define INPUTTYPES_H

#include <memory>

#include "Command.h"
#include "GamePad.h"

namespace real
{
	enum class KeyState : char
	{
		keyDown = 0,
		keyUp = 1,
		keyPressed = 2
	};

	enum class MouseButton : char
	{
		left = 1,
		middle = 2,
		right = 3
	};

	struct KeyboardAction
	{
		KeyState event;
		uint32_t scancode;
		std::unique_ptr<Command> pCommand;

		KeyboardAction(KeyState event, uint32_t scancode, Command* command)
			: event(event), scancode(scancode), pCommand(command) {}
	};

	struct MouseAction
	{
		KeyState event;
		MouseButton button;
		std::unique_ptr<Command> pCommand;

		MouseAction(KeyState event, MouseButton button, Command* command)
			: event(event), button(button), pCommand(command) {}
	};

	struct ControllerAction
	{
		uint8_t controller;
		KeyState inputType;
		GamePad::Button button;
		std::unique_ptr<Command> pCommand;

		ControllerAction(uint8_t controller, KeyState inputType, GamePad::Button button, Command* pCommand)
			: controller(controller), inputType(inputType), button(button), pCommand(pCommand) {}
	};
}

#endif // INPUTTYPES_H