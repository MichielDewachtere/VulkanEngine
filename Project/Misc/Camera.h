// ReSharper disable CppClangTidyClangDiagnosticOverloadedVirtual
#ifndef CAMERA_H
#define CAMERA_H

#include <real_core/Singleton.h>
#include "Util/Structs.h"

// TODO: Make a manager for cameras
class Camera final : public real::Singleton<Camera>
{
public:
	virtual ~Camera() override = default;

	Camera(const Camera& other) = delete;
	Camera& operator=(const Camera& rhs) = delete;
	Camera(Camera&& other) = delete;
	Camera& operator=(Camera&& rhs) = delete;

	void Init(const GameContext& context);

	void Update(const GameContext& context);

	const glm::vec3& GetPosition() const { return m_Position; }

	const glm::mat4& GetView() const { return m_View; }
	const glm::mat4& GetProjection() const { return m_Projection; }
	const glm::mat4& GetViewProjection() const { return m_ViewProjection; }
	const glm::mat4& GetViewInverse() const { return m_ViewInverse; }
	const glm::mat4& GetViewProjectionInverse() const { return m_ViewProjectionInverse; }

private:
	friend class Singleton<Camera>;
	explicit Camera() = default;

	glm::vec3 m_Position{ 0,0,2 };
	glm::vec3 m_Up{ 0,1,0 };
	glm::vec3 m_Forward{ 0,0,1 };
	glm::vec3 m_Right{ 1,0,0 };
	//glm::vec3 m_Rotation{ 0,0,0 };

	static inline constexpr glm::vec4 unit_x{ 1,0,0,0 }, unit_y{ 0,1,0,0 }, unit_z{ 0,0,1,0 };

	glm::mat4 m_View{};
	glm::mat4 m_Projection{};
	glm::mat4 m_ViewInverse{};
	glm::mat4 m_ViewProjection{};
	glm::mat4 m_ViewProjectionInverse{};

	float m_FarPlane{}, m_NearPlane{}, m_FOV{};
	float m_Pitch{}, m_Yaw{};
	float m_MoveSpeed{ 10.f }, m_SpeedMultiplier{ 2.5f }, m_RotationSpeed{ 600.f };

	void CalculateViewMatrix();
	void CalculateProjectionMatrix(const GameContext& settings);

	void HandleKeyboardInput(float dt);
	void HandleMouseInput(float dt);
};

#endif // CAMERA_H