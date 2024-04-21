#include "GamePad.h"

#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#define NOMINMAX

#include "InputManager.h"
#include "Xinput.h"

class real::GamePad::GamePadImpl
{
public:
	explicit GamePadImpl(uint8_t controllerIndex)
		: m_ControllerIndex(controllerIndex)
	{
		ZeroMemory(&m_PreviousState, sizeof XINPUT_STATE);
		ZeroMemory(&m_CurrentState, sizeof XINPUT_STATE);
	}
	~GamePadImpl() = default;

	GamePadImpl(const GamePadImpl& other) = delete;
	GamePadImpl& operator=(const GamePadImpl& rhs) = delete;
	GamePadImpl(GamePadImpl&& other) = delete;
	GamePadImpl& operator=(GamePadImpl&& rhs) = delete;

	void Update()
	{
		CopyMemory(&m_PreviousState, &m_CurrentState, sizeof XINPUT_STATE);
		ZeroMemory(&m_CurrentState, sizeof XINPUT_STATE);
		XInputGetState(m_ControllerIndex, &m_CurrentState);

		const auto buttonChanges = m_CurrentState.Gamepad.wButtons ^ m_PreviousState.Gamepad.wButtons;
		m_ButtonsPressedThisFrame = buttonChanges & m_CurrentState.Gamepad.wButtons;
		m_ButtonsReleasedThisFrame = buttonChanges & (~m_CurrentState.Gamepad.wButtons);
	}

	uint8_t GetIndex() const { return m_ControllerIndex; }

	bool IsDown(unsigned int button) const { return m_ButtonsPressedThisFrame & button; }
	bool IsUp(unsigned int button) const { return m_ButtonsReleasedThisFrame & button; }
	bool IsPressed(unsigned int button) const { return m_CurrentState.Gamepad.wButtons & button; }

private:
	XINPUT_STATE m_PreviousState{};
	XINPUT_STATE m_CurrentState{};

	WORD m_ButtonsPressedThisFrame{};
	WORD m_ButtonsReleasedThisFrame{};

	uint8_t m_ControllerIndex{};
};

real::GamePad::GamePad(uint8_t controllerIndex)
	: m_pImpl(new GamePadImpl(controllerIndex))
{
}

real::GamePad::~GamePad()
{
	delete m_pImpl;
	m_pImpl = nullptr;
}

void real::GamePad::Update()
{
	m_pImpl->Update();
}

uint8_t real::GamePad::GetIndex() const
{
	return m_pImpl->GetIndex();
}

bool real::GamePad::IsDown(Button button) const
{
	return m_pImpl->IsDown(static_cast<unsigned>(button));
}

bool real::GamePad::IsUp(Button button) const
{
	return m_pImpl->IsUp(static_cast<unsigned>(button));
}

bool real::GamePad::IsPressed(Button button) const
{
	return m_pImpl->IsPressed(static_cast<unsigned>(button));
}
