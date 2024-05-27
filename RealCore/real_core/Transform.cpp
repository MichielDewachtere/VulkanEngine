#include "Transform.h"

#include <algorithm>

#pragma warning(disable : 4201) // Disable warning 4201
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#pragma warning(default : 4201) // Restore the warning state to default

#include "GameObject.h"

// ReSharper disable once CppPossiblyUninitializedMember
real::Transform::Transform(GameObject* pOwner, TransformInfo info)
	: Component(pOwner)
	, m_Scale(info.scale)
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

	SetRotation(info.rotation, true);
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
	SetWorldMatrixDirty();

	localPosChanged.Notify(TransformEvent::localPosChanged, m_WorldPosition);

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
	SetWorldMatrixDirty();

	worldPosChanged.Notify(TransformEvent::worldPosChanged, m_WorldPosition);
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

	SetWorldPositionDirty();
	SetWorldMatrixDirty();
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
	SetWorldMatrixDirty();
	scaleChanged.Notify(TransformEvent::scaleChanged, scale);
}

const glm::vec3& real::Transform::GetEulerRotation() const
{
	glm::vec3 v;
	v.x = GetPitch();
	v.y = GetYaw();
	v.z = GetRoll();
	return v;
}

void real::Transform::SetRotation(const glm::vec3& rotation, bool degrees)
{
	glm::quat rotationQuat;

	if (degrees)
		rotationQuat = glm::quat(glm::radians(rotation));
	else
		rotationQuat = glm::quat(rotation);

	SetRotation(glm::normalize(rotationQuat));
}

void real::Transform::SetRotation(const glm::quat& rotation)
{
	m_Rotation = rotation;

	SetOrientVecDirty();
	SetRotationMatrixDirty();
	SetWorldMatrixDirty();
}

void real::Transform::SetPitch(const float pitch, bool degrees)
{
	m_Rotation = glm::normalize(glm::angleAxis(degrees ? glm::radians(pitch) : pitch, glm::vec3(unit_x)) * GetRotation());

	SetOrientVecDirty();
	SetRotationMatrixDirty();
	SetWorldMatrixDirty();
}

void real::Transform::SetYaw(const float yaw, bool degrees)
{
	m_Rotation = glm::normalize(glm::angleAxis(degrees ? glm::radians(yaw) : yaw, glm::vec3(unit_y)) * GetRotation());

	SetOrientVecDirty();
	SetRotationMatrixDirty();
	SetWorldMatrixDirty();
}

void real::Transform::SetRoll(const float roll, bool degrees)
{
	m_Rotation = glm::normalize(glm::angleAxis(degrees ? glm::radians(roll) : roll, glm::vec3(unit_z)) * GetRotation());

	SetOrientVecDirty();
	SetRotationMatrixDirty();
	SetWorldMatrixDirty();
}

const glm::mat4& real::Transform::GetWorldMatrix()
{
	if (m_WorldMatNeedsUpdate)
	{
		m_WorldMatrix = glm::mat4(1.0f);
		const auto worldPos = GetWorldPosition();

		m_WorldMatrix = glm::translate(m_WorldMatrix, worldPos);

		m_WorldMatrix *= GetRotationMatrix();

		m_WorldMatrix = glm::scale(m_WorldMatrix, m_Scale);

		m_WorldMatNeedsUpdate = false;
	}

	return m_WorldMatrix;
}

const glm::mat4& real::Transform::GetRotationMatrix()
{
	if (m_RotationMatNeedsUpdate)
	{
		m_RotationMatrix = glm::mat4{ GetRotation() };
	}

	return m_RotationMatrix;
}

const glm::vec3& real::Transform::GetUp()
{
	if (m_UpNeedsUpdate)
	{
		m_Up = glm::vec3(GetRotationMatrix() * unit_y);
		m_UpNeedsUpdate = false;
	}

	return m_Up;
}

const glm::vec3& real::Transform::GetForward()
{
	if (m_ForwardNeedsUpdate)
	{
		m_Forward = glm::vec3(GetRotationMatrix() * unit_z);
		m_ForwardNeedsUpdate = false;
	}

	return m_Forward;
}

const glm::vec3& real::Transform::GetRight()
{
	if (m_RightNeedsUpdate)
	{
		m_Right = glm::vec3(GetRotationMatrix() * unit_x);
		m_RightNeedsUpdate = false;
	}

	return m_Right;
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

void real::Transform::SetWorldMatrixDirty()
{
	m_WorldMatNeedsUpdate = true;

	std::ranges::for_each(GetOwner()->GetChildren(), [](GameObject* go)
		{
			go->GetTransform()->SetWorldMatrixDirty();
		});
}

void real::Transform::SetOrientVecDirty()
{
	m_ForwardNeedsUpdate = true;
	m_UpNeedsUpdate = true;
	m_RightNeedsUpdate = true;

	std::ranges::for_each(GetOwner()->GetChildren(), [](GameObject* go)
		{
			go->GetTransform()->SetOrientVecDirty();
		});
}

void real::Transform::SetRotationMatrixDirty()
{
	m_RotationMatNeedsUpdate = true;

	std::ranges::for_each(GetOwner()->GetChildren(), [](GameObject* go)
		{
			go->GetTransform()->SetRotationMatrixDirty();
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
