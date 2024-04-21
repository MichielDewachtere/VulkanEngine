#ifndef GAMEPAD_H
#define GAMEPAD_H

#include <stdint.h>

namespace real
{
	class GamePad final
	{
	public:
		enum class Button
		{
			dPadUp = 0x0001,
			dPadDown = 0x0002,
			dPadLeft = 0x0004,
			dPadRight = 0x0008,
			start = 0x0010,
			back = 0x0020,
			leftThumb = 0x0040,
			rightThumb = 0x0080,
			leftShoulder = 0x0100,
			rightShoulder = 0x0200,
			buttonDown = 0x1000,
			buttonRight = 0x2000,
			buttonLeft = 0x4000,
			buttonUp = 0x8000
		};

		explicit GamePad(uint8_t controllerIndex);
		~GamePad();

		GamePad(const GamePad& other) = delete;
		GamePad& operator=(const GamePad& rhs) = delete;
		GamePad(GamePad&& other) = delete;
		GamePad& operator=(GamePad&& rhs) = delete;

		void Update();

		uint8_t GetIndex() const;

		bool IsDown(Button button) const;
		bool IsUp(Button button) const;
		bool IsPressed(Button button) const;

	private:
		class GamePadImpl;
		GamePadImpl* m_pImpl;
	};
}

#endif // GAMEPAD_H