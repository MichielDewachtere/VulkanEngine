#include "MoveCommand.h"

#include <glm/gtc/matrix_transform.hpp>

#include <real_core/GameObject.h>
#include <real_core/GameTime.h>

#include "Components/Player.h"

MoveCommand::MoveCommand(int id, int controllerId, real::GameObject* pOwner, glm::ivec3 direction)
	: GameObjectCommand(id, controllerId, pOwner)
	, m_Direction(direction)
{
	m_Direction = glm::clamp(m_Direction, glm::ivec3(-1), glm::ivec3(1));
	m_pPlayerComponent = GetGameObject()->GetComponent<Player>();
}

void MoveCommand::Execute()
{
	const auto dt = real::GameTime::GetInstance().GetElapsed();
	const auto transform = GetGameObject()->GetTransform();

	glm::vec3 translation{ 0,0,0 };
	
	// Normalize the forward vector in the x-z plane
	const auto forwardXz = glm::normalize(glm::vec3(transform->GetForward().x, 0.0f, transform->GetForward().z));
	const auto rightXz = glm::normalize(glm::cross(glm::vec3(0, 1, 0), forwardXz));

	translation += forwardXz * dt * m_Speed * static_cast<float>(m_Direction.z);
	translation += glm::vec3(0, 1, 0) * dt * m_Speed * static_cast<float>(m_Direction.y);
	translation += rightXz * dt * m_Speed * static_cast<float>(m_Direction.x);

	transform->Translate(translation);

	const auto pos = transform->GetWorldPosition();
	m_pPlayerComponent->UpdateChunkPos(glm::vec3(pos));
}