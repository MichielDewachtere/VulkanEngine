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

	struct KeyboardAction
	{
		KeyState event;
		uint32_t scancode;
		std::unique_ptr<Command> pCommand;

		KeyboardAction(KeyState event, uint32_t scancode, Command* command)
			: event(event), scancode(scancode), pCommand(command) {}
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