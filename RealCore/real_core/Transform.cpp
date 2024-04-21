#include "Transform.h"

#include <algorithm>

#include "GameObject.h"

real::Transform::Transform(GameObject* pOwner, glm::vec3 localPos)
	: Component(pOwner)
	, m_LocalPosition(localPos)
{
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

const glm::vec2& real::Transform::GetLocalPosition()
{
	if (m_LocalNeedsUpdate)
		UpdateLocalPosition();

	return m_LocalPosition;
}

void real::Transform::SetLocalPosition(const glm::vec2& localPos)
{
	m_LocalPosition = localPos;
	SetWorldPositionDirty();

	localPosChanged.Notify(TransformEvent::localPosChanged, m_WorldPosition);

	std::ranges::for_each(GetOwner()->GetChildren(), [](GameObject* go)
		{
			go->GetTransform()->SetLocalPositionDirty();
		});
}

void real::Transform::SetLocalPosition(const float x, const float y)
{
	SetLocalPosition({ x,y });
}

const glm::vec2& real::Transform::GetWorldPosition()
{
	if (m_WorldNeedsUpdate)
		UpdateWorldPosition();

	return m_WorldPosition;
}

void real::Transform::SetWorldPosition(const glm::vec2& worldPos)
{
	m_WorldPosition = worldPos;
	SetLocalPositionDirty();

	worldPosChanged.Notify(TransformEvent::worldPosChanged, m_WorldPosition);

	std::ranges::for_each(GetOwner()->GetChildren(), [](GameObject* go)
		{
			go->GetTransform()->SetWorldPositionDirty();
		});
}

void real::Transform::SetWorldPosition(const float x, const float y)
{
	SetWorldPosition({ x,y });
}

void real::Transform::Translate(const glm::vec2& translation)
{
	m_LocalPosition += translation;
	localPosChanged.Notify(TransformEvent::localPosChanged, m_LocalPosition);

	m_WorldNeedsUpdate = true;
	if (worldPosChanged.GetObservers().empty() == false)
	{
		GetWorldPosition();
		worldPosChanged.Notify(TransformEvent::worldPosChanged, m_WorldPosition);
	}
}

void real::Transform::Translate(float translationX, float translationY)
{
	Translate({ translationX, translationY });
}

void real::Transform::SetUniformScale(float s)
{
	SetScale({ s,s });
}

void real::Transform::SetScale(float x, float y)
{
	SetScale({ x,y });
}

void real::Transform::SetScale(const glm::vec2& scale)
{
	m_Scale = scale;
	scaleChanged.Notify(TransformEvent::scaleChanged, scale);
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

//void real::Transform::SetRotation(float rotation)
//{
//	m_Rotation = rotation;
//}
//
//void real::Transform::IncrementRotation(const float amount)
//{
//	m_Rotation += amount;
//}
//
//void real::Transform::DecrementRotation(const float amount)
//{
//	m_Rotation -= amount;
//}