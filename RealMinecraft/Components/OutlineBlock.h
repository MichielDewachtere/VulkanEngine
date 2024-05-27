#ifndef OUTLINEBLOCK_H
#define OUTLINEBLOCK_H

#include <real_core/Component.h>

#include "Mesh/MeshIndexed.h"

class World;

namespace real
{
	class Transform;
}

class OutlineBlock final : public real::Component
{
public:
	explicit OutlineBlock(real::GameObject* pOwner, real::GameObject* player, World* world, int reach);
	~OutlineBlock() override = default;

	OutlineBlock(const OutlineBlock& other) = delete;
	OutlineBlock& operator=(const OutlineBlock& rhs) = delete;
	OutlineBlock(OutlineBlock&& other) = delete;
	OutlineBlock& operator=(OutlineBlock&& rhs) = delete;

	virtual void Start() override;
	virtual void Update() override;

	bool HasBlockSelected() const { return m_HasBlockSelected; };
	std::pair<glm::ivec2, glm::ivec3> GetSelectedBlock() const { return m_SelectedBlock; }
	std::pair<glm::ivec2, glm::ivec3> GetPosToPlace() const { return m_CanPlaceAt; }

private:
	real::Transform* m_pPlayerTransform;
	World* m_pWorldComponent;
	int m_Reach;

	bool m_HasBlockSelected{ false };
	std::pair<glm::ivec2, glm::ivec3> m_SelectedBlock;
	std::pair<glm::ivec2, glm::ivec3> m_CanPlaceAt;

	real::MeshIndexed<real::PosColNorm, real::UniformBufferObject>* m_pMeshComponent{ nullptr };
};


#endif // OUTLINEBLOCK_H