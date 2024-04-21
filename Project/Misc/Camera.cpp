// ReSharper disable CppRedundantQualifier
#include "Camera.h"

#include <real_core/GameTime.h>
#include <real_core/Logger.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "InputManager.h"
#include "Graphics/Renderer.h"
#include "Core/SwapChain.h"

void Camera::Init(const GameContext& context)
{
	m_FarPlane = 100.0f;
	m_NearPlane = 0.1f;
	m_FOV = 45.0f;

	m_Projection = glm::mat4(1.f);
	m_View = glm::mat4(1.f);
	m_ViewInverse = glm::mat4(1.f);
	m_ViewProjection = glm::mat4(1.f);
	m_ViewProjectionInverse = glm::mat4(1.f);

	real::Logger::LogInfo({ "Test" });

	CalculateProjectionMatrix(context);
}

void Camera::Update(const GameContext& /*context*/)
{
	// Handle Input
	const auto elapsedTime = real::GameTime::GetInstance().GetElapsed();
	HandleKeyboardInput(elapsedTime);
	HandleMouseInput(elapsedTime);

	// Calculate rotation quaternion
	const auto rot = glm::quat(glm::vec3(glm::radians<float>(m_Pitch), glm::radians<float>(m_Yaw), 0.0f));

	// Create rotation matrix
	const auto rotMat = glm::mat4_cast(rot);

	// Transform unit vectors by rotation matrix
	m_Forward = glm::vec3(rotMat * unit_z);
	m_Right = glm::vec3(rotMat * unit_x);
	m_Up = glm::cross(m_Forward, m_Right);

	// Calculate view matrix
	CalculateViewMatrix();

	// Combine view and projection matrices
	const auto viewProjection = m_View * m_Projection;

	// Calculate the inverse of viewProjection
	const auto viewProjectionInv = glm::inverse(viewProjection);

	// Assign the matrices
	m_ViewProjection = viewProjection;
	m_ViewProjectionInverse = viewProjectionInv;
}

void Camera::CalculateViewMatrix()
{
	//ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	const auto target = m_Position - m_Forward;
	m_View = glm::lookAt(m_Position, target, m_Up);
	m_ViewInverse = glm::inverse(m_View);
}

void Camera::CalculateProjectionMatrix(const GameContext& /*settings*/)
{
	const auto [width, height] = Renderer::GetInstance().GetSwapChain()->GetExtent();
	const float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
	m_Projection = glm::perspective(glm::radians(m_FOV), aspectRatio, m_NearPlane, m_FarPlane);
}

void Camera::HandleKeyboardInput(float dt)
{
	auto& input = InputManager::GetInstance();

	glm::vec3 move{ 0,0,0 };
	move.y = -(input.IsKeyboardKey(InputState::down, SDL_SCANCODE_W) ? 1.0f : 0.0f);
	if ((int)move.y == 0)
		move.y = input.IsKeyboardKey(InputState::down, SDL_SCANCODE_S) ? 1.0f : 0.0f;

	move.x = input.IsKeyboardKey(InputState::down, SDL_SCANCODE_D) ? 1.0f : 0.0f;
	if ((int)move.x == 0)
		move.x = -(input.IsKeyboardKey(InputState::down, SDL_SCANCODE_A) ? 1.0f : 0.0f);

	move.z = input.IsKeyboardKey(InputState::down, SDL_SCANCODE_SPACE) ? 1.0f : 0.0f;
	if ((int)move.z == 0)
		move.z = -(input.IsKeyboardKey(InputState::down, SDL_SCANCODE_LSHIFT) ? 1.0f : 0.0f);

	float speed = m_MoveSpeed;
	if (input.IsKeyboardKey(InputState::down, SDL_SCANCODE_Q))
		speed *= m_SpeedMultiplier;

	m_Position += m_Forward * move.y * speed * dt;
	m_Position += glm::vec3(unit_y) * move.z * speed * dt;
	m_Position += m_Right * move.x * speed * dt;

}

void Camera::HandleMouseInput(float dt)
{
	glm::vec2 look{ 0,0 };
	if (InputManager::GetInstance().IsMouseButton(InputState::down, MouseButton::left))
	{
		const auto& v = InputManager::GetInstance().GetMouseMovement();
		look.x = static_cast<float>(v.x);
		look.y = static_cast<float>(v.y);
	}

	m_Yaw += look.x * m_RotationSpeed * dt;
	m_Pitch += look.y * m_RotationSpeed * dt;
}
