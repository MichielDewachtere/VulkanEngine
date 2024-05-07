#include "Transform.h"

#include <algorithm>

#pragma warning(disable : 4201) // Disable warning 4201
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#pragma warning(default : 4201) // Restore the warning state to default

#include "GameObject.h"

// ReSharper disable once CppPossiblyUninitializedMember
real::Transform::Transform(GameObject* pOwner, TransformInfo info)
	: Component(pOwner)
	, m_Scale(info.scale)
	, m_Rotation(info.rotation)
{
	if (info.useLocalPosition)
	{
		m_LocalPosition = info.position;
		m_WorldNeedsUpdate = true;
		m_LocalNeedsUpdate = false;
	}
	else if (info.useLocalPosition == false)
	{
		m_WorldPosition = info.position;
		m_LocalNeedsUpdate = true;
		m_WorldNeedsUpdate = false;
	}
}

void real::Transform::Start()
{
}

void real::Transform::Update()
{
	if (m_WorldNeedsUpdate)
		UpdateWorldPosition();

	if (m_LocalNeedsUpdate)
		UpdateLocalPosition();
}

const glm::vec3& real::Transform::GetLocalPosition()
{
	if (m_LocalNeedsUpdate)
		UpdateLocalPosition();

	return m_LocalPosition;
}

void real::Transform::SetLocalPosition(const glm::vec2& localPos)
{
	SetLocalPosition(glm::vec3{ localPos.x, localPos.y, m_LocalPosition.z });
}

void real::Transform::SetLocalPosition(const glm::vec3& localPos)
{
	m_LocalPosition = localPos;
	SetWorldPositionDirty();

	localPosChanged.Notify(TransformEvent::localPosChanged, m_WorldPosition);
	m_WorldMatNeedsUpdate = true;
	std::ranges::for_each(GetOwner()->GetChildren(), [](GameObject* go)
		{
			go->GetTransform()->SetLocalPositionDirty();
		});
}

void real::Transform::SetLocalPosition(const float x, const float y)
{
	SetLocalPosition(glm::vec3{ x,y, m_LocalPosition.z });
}

void real::Transform::SetLocalPosition(float x, float y, float z)
{
	SetLocalPosition(glm::vec3{ x,y,z });
}

const glm::vec3& real::Transform::GetWorldPosition()
{
	if (m_WorldNeedsUpdate)
		UpdateWorldPosition();

	return m_WorldPosition;
}

void real::Transform::SetWorldPosition(const glm::vec2& worldPos)
{
	SetWorldPosition(glm::vec3{ worldPos.x, worldPos.y, m_WorldPosition.z });
}

void real::Transform::SetWorldPosition(const glm::vec3& worldPos)
{
	m_WorldPosition = worldPos;
	SetLocalPositionDirty();

	worldPosChanged.Notify(TransformEvent::worldPosChanged, m_WorldPosition);
	m_WorldMatNeedsUpdate = true;
	std::ranges::for_each(GetOwner()->GetChildren(), [](GameObject* go)
		{
			go->GetTransform()->SetWorldPositionDirty();
		});
}

void real::Transform::SetWorldPosition(const float x, const float y)
{
	SetWorldPosition(glm::vec3{ x,y, m_WorldPosition.z });
}

void real::Transform::SetWorldPosition(float x, float y, float z)
{
	SetWorldPosition(glm::vec3{ x,y,z });
}

void real::Transform::Translate(const glm::vec2& translation)
{
	Translate(glm::vec3{ translation.x,translation.y,0 });
}

void real::Transform::Translate(const glm::vec3& translation)
{
	m_LocalPosition += translation;
	localPosChanged.Notify(TransformEvent::localPosChanged, m_LocalPosition);

	m_WorldNeedsUpdate = true;
	m_WorldMatNeedsUpdate = true;
	if (worldPosChanged.GetObservers().empty() == false)
	{
		GetWorldPosition();
		worldPosChanged.Notify(TransformEvent::worldPosChanged, m_WorldPosition);
	}
}

void real::Transform::Translate(float translationX, float translationY)
{
	Translate({ translationX, translationY, 0 });
}

void real::Transform::Translate(float x, float y, float z)
{
	Translate({ x, y, z});
}

void real::Transform::SetUniformScale(float s)
{
	SetScale({ s,s,s });
}

void real::Transform::SetScale(float x, float y)
{
	SetScale(glm::vec3{ x,y,m_Scale.z });
}

void real::Transform::SetScale(float x, float y, float z)
{
	SetScale(glm::vec3{ x,y,z });
}

void real::Transform::SetScale(const glm::vec2& scale)
{
	SetScale({ scale.x, scale.y, m_Scale.z });
}

void real::Transform::SetScale(const glm::vec3& scale)
{
	m_Scale = scale;
	m_WorldMatNeedsUpdate = true;
	scaleChanged.Notify(TransformEvent::scaleChanged, scale);
}

void real::Transform::SetRotation(const glm::vec3& rotation, bool degrees)
{
	if (degrees)
		m_Rotation = glm::radians(rotation);
	else
		m_Rotation = rotation;

	m_WorldMatNeedsUpdate = true;
}

void real::Transform::SetPitch(const float pitch, bool degrees)
{
	if (degrees)
		m_Rotation.x = glm::radians(pitch);
	else
		m_Rotation.x = pitch;

	m_WorldMatNeedsUpdate = true;
}

void real::Transform::SetYaw(const float yaw, bool degrees)
{
	if (degrees)
		m_Rotation.y = glm::radians(yaw);
	else
		m_Rotation.y = yaw;

	m_WorldMatNeedsUpdate = true;
}

void real::Transform::SetRoll(const float roll, bool degrees)
{
	if (degrees)
		m_Rotation.z = glm::radians(roll);
	else
		m_Rotation.z = roll;

	m_WorldMatNeedsUpdate = true;
}

const glm::mat4& real::Transform::GetWorldMatrix()
{
	if (m_WorldMatNeedsUpdate)
	{
		m_WorldMatrix = glm::mat4(1.0f);
		const auto worldPos = GetWorldPosition();

		m_WorldMatrix = glm::translate(m_WorldMatrix, worldPos);

		m_WorldMatrix = glm::rotate(m_WorldMatrix, m_Rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		m_WorldMatrix = glm::rotate(m_WorldMatrix, m_Rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		m_WorldMatrix = glm::rotate(m_WorldMatrix, m_Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

		m_WorldMatrix = glm::scale(m_WorldMatrix, m_Scale);

		m_WorldMatNeedsUpdate = false;
	}

	return m_WorldMatrix;
}

void real::Transform::UpdateLocalPosition()
{
	m_LocalNeedsUpdate = false;

	if (GetOwner()->GetParent() == nullptr)
		m_LocalPosition = m_WorldPosition;
	else
		m_LocalPosition = m_WorldPosition - GetOwner()->GetParent()->GetTransform()->GetWorldPosition();

	localPosChanged.Notify(TransformEvent::localPosChanged, m_LocalPosition);
}

void real::Transform::SetWorldPositionDirty()
{
	m_WorldNeedsUpdate = true;

	std::ranges::for_each(GetOwner()->GetChildren(), [](GameObject* go)
		{
			go->GetTransform()->SetWorldPositionDirty();
		});
}

void real::Transform::SetLocalPositionDirty()
{
	m_LocalNeedsUpdate = true;

	std::ranges::for_each(GetOwner()->GetChildren(), [](GameObject* go)
		{
			go->GetTransform()->SetLocalPositionDirty();
		});
}

void real::Transform::UpdateWorldPosition()
{
	m_WorldNeedsUpdate = false;

	if (GetOwner()->GetParent() == nullptr)
		m_WorldPosition = m_LocalPosition;
	else
		m_WorldPosition = GetOwner()->GetParent()->GetTransform()->GetWorldPosition() + m_LocalPosition;

	worldPosChanged.Notify(TransformEvent::worldPosChanged, m_WorldPosition);
}