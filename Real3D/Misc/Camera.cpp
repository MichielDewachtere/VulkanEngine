// ReSharper disable CppRedundantQualifier
#include "Camera.h"

#include <real_core/GameObject.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//#include "CameraInputManager.h"
#include "RealEngine.h"
#include "Graphics/Renderer.h"
#include "Core/SwapChain.h"

real::Camera::Camera(real::GameObject* pOwner)
	: Component(pOwner)
{
	Init(RealEngine::GetGameContext());
}

void real::Camera::Update()
{
	auto context = RealEngine::GetGameContext();

	// Calculate view matrix
	CalculateViewMatrix();

	// Combine view and projection matrices
	m_ViewProjection = m_Projection * m_View;
	// Calculate the inverse of viewProjection
	m_ViewProjectionInverse = glm::inverse(m_ViewProjection);
}

void real::Camera::Init(const GameContext& context)
{
	m_FarFrustumPlane = 1000.0f;
	m_NearFrustumPlane = 0.1f;
	m_FOV = 45.0f;

	m_Projection = glm::mat4(1.f);
	m_View = glm::mat4(1.f);
	m_ViewInverse = glm::mat4(1.f);
	m_ViewProjection = glm::mat4(1.f);
	m_ViewProjectionInverse = glm::mat4(1.f);

	CalculateProjectionMatrix(context);
}

void real::Camera::CalculateViewMatrix()
{
	const auto transform = GetOwner()->GetTransform();
	const auto worldPos = transform->GetWorldPosition();

	const auto target = worldPos - transform->GetForward();
	m_View = glm::lookAt(worldPos, target, transform->GetUp());
	m_ViewInverse = glm::inverse(m_View);
}

void real::Camera::CalculateProjectionMatrix(const GameContext& /*settings*/)
{
	const auto [width, height] = Renderer::GetInstance().GetSwapChain()->GetExtent();
	const float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
	m_Projection = glm::perspective(glm::radians(m_FOV), aspectRatio, m_NearFrustumPlane, m_FarFrustumPlane);
}