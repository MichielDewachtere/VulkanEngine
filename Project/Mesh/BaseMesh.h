#ifndef BASEMESH_H
#define BASEMESH_H

#include "Util/Structs.h"

class Material;

class BaseMesh
{
public:
	virtual ~BaseMesh() = default;

	virtual void Init(const GameContext& context) = 0;
	virtual void CleanUp(const GameContext& context) = 0;

	virtual void Update(uint32_t currentFrame) = 0;
	virtual void Draw(VkCommandBuffer commandBuffer) = 0;

	virtual void LinkMaterial(Material* pMaterial) {}
};

#endif // BASEMESH_H