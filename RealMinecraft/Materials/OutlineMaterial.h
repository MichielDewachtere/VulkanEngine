#ifndef OUTLINEMATERIAL_H
#define OUTLINEMATERIAL_H

#include "Material/Material.h"
#include "Util/Structs.h"

class OutlineMaterial final : public real::Material<real::UniformBufferObject>
{
public:
	explicit OutlineMaterial() = default;
	virtual ~OutlineMaterial() override = default;

	OutlineMaterial(const OutlineMaterial&) = delete;
	OutlineMaterial& operator=(const OutlineMaterial&) = delete;
	OutlineMaterial(OutlineMaterial&&) = delete;
	OutlineMaterial& operator=(OutlineMaterial&&) = delete;

	void UpdateShaderVariables(const real::DrawableComponent* mesh, uint32_t reference) override;

protected:
	void CreatePipeline() override;
	void CreateDescriptorSetLayout() override;
	void CreateDescriptorSets(uint32_t reference, VkDescriptorSetLayout layout) override;
};

#endif // OUTLINEMATERIAL_H