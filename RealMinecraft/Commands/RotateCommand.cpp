#include "RotateCommand.h"

#include <real_core/InputManager.h>
#include <real_core/GameObject.h>
#include <real_core/GameTime.h>
#include <real_core/Utils.h>

RotateCommand::RotateCommand(int id, int controllerId, real::GameObject* pOwner)
	: GameObjectCommand(id, controllerId, pOwner) {}

void RotateCommand::Execute()
{
	const auto dt = real::GameTime::GetInstance().GetElapsed();
	const auto v = real::InputManager::GetInstance().GetNormalizedMouseMovement();

	const glm::vec2 look = glm::vec2(v.y, v.x) * m_RotationSpeed * dt;

	m_Pitch += look.x;
	m_Yaw += look.y;

	const glm::quat pitchQuat = glm::angleAxis(m_Pitch, glm::vec3(1.0f, 0.0f, 0.0f));
	const glm::quat yawQuat = glm::angleAxis(m_Yaw, glm::vec3(0.0f, 1.0f, 0.0f));
	const glm::quat orientation = glm::normalize(yawQuat * pitchQuat);

	if (const auto prevOrientation = GetGameObject()->GetTransform()->GetRotation(); 
		prevOrientation != orientation)
		GetGameObject()->GetTransform()->SetRotation(orientation);
}