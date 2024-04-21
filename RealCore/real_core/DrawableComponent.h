#ifndef DRAWABLECOMPONENT_H
#define DRAWABLECOMPONENT_H

#include "Component.h"

namespace real
{
	class DrawableComponent : public Component
	{
	public:
		explicit DrawableComponent(GameObject* pOwner) :Component(pOwner) {}
		virtual ~DrawableComponent() override = default;

		DrawableComponent(const DrawableComponent& other) = delete;
		DrawableComponent& operator=(const DrawableComponent& rhs) = delete;
		DrawableComponent(DrawableComponent&& other) = delete;
		DrawableComponent& operator=(DrawableComponent&& rhs) = delete;

		virtual void Render() {}
		virtual void DebugRender() {}

	private:

	};
}
#endif // DRAWABLECOMPONENT_H