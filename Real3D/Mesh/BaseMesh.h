#ifndef	BASEMESH_H
#define BASEMESH_H

#include <cstdint>

#include <real_core/DrawableComponent.h>

#include "Material/Material.h"

namespace real
{
	class Texture2D;

	struct MeshInfo
	{
		uint32_t vertexCapacity = 0, indexCapacity = 0;
		Texture2D* texture = nullptr;
		bool usesUbo = false;
	};

	template <typename T>
	struct BufferContext
	{
		bool isDirty = false;
		VkBuffer buffer{ nullptr };
		VmaAllocation allocation{ nullptr };
		std::vector<T> data{};
	};

	class BaseMesh : public DrawableComponent
	{
	public:
		explicit BaseMesh(real::GameObject* pOwner, MeshInfo info)
			: DrawableComponent(pOwner) , m_Info(info) {}
		virtual ~BaseMesh() override = default;

		BaseMesh(const BaseMesh&) = delete;
		BaseMesh& operator=(const BaseMesh&) = delete;
		BaseMesh(BaseMesh&&) = delete;
		BaseMesh& operator=(BaseMesh&&) = delete;

	protected:
		MeshInfo m_Info;
	};
}

#endif // BASEMESH_H