#ifndef TRANSPARENTMATERIAL_H
#define TRANSPARENTMATERIAL_H

#include "Material/Material.h"
#include "Util/Structs.h"

class TransparentMaterial final : public real::Material<real::UniformBufferObject>
{
public:
	explicit TransparentMaterial() = default;
	virtual ~TransparentMaterial() override = default;

	TransparentMaterial(const TransparentMaterial&) = delete;
	TransparentMaterial& operator=(const TransparentMaterial&) = delete;
	TransparentMaterial(TransparentMaterial&&) = delete;
	TransparentMaterial& operator=(TransparentMaterial&&) = delete;

	void UpdateShaderVariables(const real::DrawableComponent* mesh, uint32_t reference) override;

protected:
	void CreatePipeline() override;
	void CreateDescriptorSetLayout() override;
	void CreateDescriptorSets(uint32_t reference, VkDescriptorSetLayout layout) override;
};

#endif // TRANSPARENTMATERIAL_H