// ReSharper disable CppClangTidyClangDiagnosticOverloadedVirtual
#ifndef CAMERA_H
#define CAMERA_H

#include <real_core/Component.h>
#include "Util/Structs.h"

namespace real
{
	class Camera final : public real::Component
	{
	public:
		explicit Camera(real::GameObject* pOwner);
		virtual ~Camera() override = default;

		Camera(const Camera& other) = delete;
		Camera& operator=(const Camera& rhs) = delete;
		Camera(Camera&& other) = delete;
		Camera& operator=(Camera&& rhs) = delete;

		virtual void Update() override;

		const glm::mat4& GetView() const { return m_View; }
		const glm::mat4& GetProjection() const { return m_Projection; }
		const glm::mat4& GetViewProjection() const { return m_ViewProjection; }
		const glm::mat4& GetViewInverse() const { return m_ViewInverse; }
		const glm::mat4& GetViewProjectionInverse() const { return m_ViewProjectionInverse; }

	private:
		glm::mat4 m_View{};
		glm::mat4 m_Projection{};
		glm::mat4 m_ViewInverse{};
		glm::mat4 m_ViewProjection{};
		glm::mat4 m_ViewProjectionInverse{};

		float m_FarPlane{}, m_NearPlane{}, m_FOV{};
		float m_MoveSpeed{ 10.f }, m_SpeedMultiplier{ 2.5f }, m_RotationSpeed{ 600.f };

		void Init(const GameContext& context);

		void CalculateViewMatrix();
		void CalculateProjectionMatrix(const GameContext& settings);
	};
}

#endif // CAMERA_H