#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include "Component.h"
#include "Subject.h"

namespace real
{
	// TODO: use this to initialize component, apply this to other components as well
	struct TransformInfo
	{
		glm::vec3 position{ 0,0,0 };
		bool useLocalPosition = true;
		glm::vec3 scale{ 1,1,1 };
		glm::vec3 rotation{ 0,0,0 };
	};

	enum class TransformEvent : char
	{
		worldPosChanged = 0,
		localPosChanged = 1,
		scaleChanged = 2,
		rotationChanged = 3
	};

	class Transform final : public Component
	{
	public:
		explicit Transform(GameObject* pOwner, TransformInfo info = {});
		virtual ~Transform() override = default;

		Transform(const Transform& other) = delete;
		Transform& operator=(const Transform& rhs) = delete;
		Transform(Transform&& other) = delete;
		Transform& operator=(Transform&& rhs) = delete;

		virtual void Start() override;
		virtual void Update() override;

		const glm::vec3& GetLocalPosition();
		void SetLocalPosition(const glm::vec2& localPos);
		void SetLocalPosition(const glm::vec3& localPos);
		void SetLocalPosition(float x, float y);
		void SetLocalPosition(float x, float y, float z);

		const glm::vec3& GetWorldPosition();
		void SetWorldPosition(const glm::vec2& worldPos);
		void SetWorldPosition(const glm::vec3& worldPos);
		void SetWorldPosition(float x, float y);
		void SetWorldPosition(float x, float y, float z);

		void Translate(float x, float y);
		void Translate(float x, float y, float z);
		void Translate(const glm::vec2&);
		void Translate(const glm::vec3&);

		const glm::vec3 GetScale() const { return m_Scale; }
		void SetUniformScale(float s);
		void SetScale(float x, float y);
		void SetScale(float x, float y, float z);
		void SetScale(const glm::vec2& scale);
		void SetScale(const glm::vec3& scale);

		/**
		 * \brief Returns the rotations vector (pitch, yaw, roll)
		 * \return Returns the pitch, yaw and roll in RADIANS
		 */
		const glm::vec3& GetRotation() const { return m_Rotation; }
		void SetRotation(const glm::vec3& rotation, bool degrees = true);
		/**
		 * \brief Sets the rotation around the x-axis
		 * \param pitch rotation around x-axis
		 * \param degrees is pitch in degrees?
		 */
		void SetPitch(const float pitch, bool degrees = true);
		/**
		 * \brief Sets the rotation around the y-axis
		 * \param yaw rotation around y-axis
		 * \param degrees is yaw in degrees?
		 */
		void SetYaw(const float yaw, bool degrees = true);
		/**
		 * \brief Sets the rotation around the z-axis
		 * \param roll rotation around z-axis
		 * \param degrees is roll in degrees?
		 */
		void SetRoll(const float roll, bool degrees = true);

		const glm::mat4& GetWorldMatrix();

		Subject<TransformEvent, const glm::vec3&> worldPosChanged;
		Subject<TransformEvent, const glm::vec3&> localPosChanged;
		Subject<TransformEvent, const glm::vec3&> scaleChanged;
		Subject<TransformEvent, const glm::vec3&> rotationChanged;

	private:
		bool m_LocalNeedsUpdate, m_WorldNeedsUpdate, m_WorldMatNeedsUpdate;
		glm::vec3 m_LocalPosition, m_WorldPosition, m_Scale, m_Rotation;
		glm::mat4 m_WorldMatrix{ 1.0 };

		void UpdateWorldPosition();
		void UpdateLocalPosition();
		void SetWorldPositionDirty();
		void SetLocalPositionDirty();
	};
}

#endif // TRANSFORM_H