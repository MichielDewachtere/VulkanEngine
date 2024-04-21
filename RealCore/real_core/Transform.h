#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "Component.h"
#include "Subject.h"

namespace real
{
	enum class TransformEvent : char
	{
		worldPosChanged = 0,
		localPosChanged = 1,
		scaleChanged = 2
	};

	class Transform final : public Component
	{
	public:
		explicit Transform(GameObject* pOwner) : Component(pOwner) {}
		explicit Transform(GameObject* pOwner, glm::vec3 localPos);
		virtual ~Transform() override = default;

		Transform(const Transform& other) = delete;
		Transform& operator=(const Transform& rhs) = delete;
		Transform(Transform&& other) = delete;
		Transform& operator=(Transform&& rhs) = delete;

		virtual void Start() override;
		virtual void Update() override;

		const glm::vec2& GetLocalPosition();
		void SetLocalPosition(const glm::vec2& localPos);
		void SetLocalPosition(float x, float y);

		const glm::vec2& GetWorldPosition();
		void SetWorldPosition(const glm::vec2& );
		void SetWorldPosition(float x, float y);

		void Translate(const glm::vec2&);
		void Translate(float x, float y);

		const glm::vec2 GetScale() const { return m_Scale; }
		void SetUniformScale(float s);
		void SetScale(float x, float y);
		void SetScale(const glm::vec2& scale);

		//const glm::vec2& GetPivot() const { return m_Pivot; }
		//void SetPivot(const glm::vec2& pivot) { m_Pivot = pivot; }
		//void SetPivot(float x, float y) { SetPivot({ x,y }); }

		//const float GetRotation() const { return m_Rotation; }
		//void SetRotation(float rotation);
		//void IncrementRotation(const float amount);
		//void DecrementRotation(const float amount);

		Subject<TransformEvent, const glm::vec2&> worldPosChanged;
		Subject<TransformEvent, const glm::vec2&> localPosChanged;
		Subject<TransformEvent, const glm::vec2&> scaleChanged;

	private:
		bool m_LocalNeedsUpdate{ true }, m_WorldNeedsUpdate{ true };
		glm::vec2 m_LocalPosition{ 0,0 }, m_WorldPosition{ 0,0 }, m_Scale{ 1,1 };
		//float m_Rotation{ 0.f };

		void UpdateWorldPosition();
		void UpdateLocalPosition();
		void SetWorldPositionDirty();
		void SetLocalPositionDirty();
	};
}

#endif // TRANSFORM_H