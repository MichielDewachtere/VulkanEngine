#ifndef DIFFUSEMATERIAL_H
#define DIFFUSEMATERIAL_H

#include "Material/Material.h"
#include "Util/Structs.h"

class DiffuseMaterial final : public real::Material<real::UniformBufferObject>
{
public:
	explicit DiffuseMaterial() = default;
	virtual ~DiffuseMaterial() override = default;

	DiffuseMaterial(const DiffuseMaterial&) = delete;
	DiffuseMaterial& operator=(const DiffuseMaterial&) = delete;
	DiffuseMaterial(DiffuseMaterial&&) = delete;
	DiffuseMaterial& operator=(DiffuseMaterial&&) = delete;

	void UpdateShaderVariables(const real::DrawableComponent* mesh, uint32_t reference) override;

protected:
	void CreatePipeline() override;
	void CreateDescriptorSetLayout() override;
	void CreateDescriptorSets(uint32_t reference, VkDescriptorSetLayout layout) override;
};

#endif // DIFFUSEMATERIAL_H