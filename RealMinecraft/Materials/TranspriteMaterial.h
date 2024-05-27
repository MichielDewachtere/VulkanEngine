#ifndef TRANSPRITEMATERIAL_H
#define TRANSPRITEMATERIAL_H

#include "Material/Material.h"
#include "Util/Structs.h"

class TranspriteMaterial final : public real::Material<real::UniformBufferObject>
{
public:
	explicit TranspriteMaterial() = default;
	virtual ~TranspriteMaterial() override = default;

	TranspriteMaterial(const TranspriteMaterial&) = delete;
	TranspriteMaterial& operator=(const TranspriteMaterial&) = delete;
	TranspriteMaterial(TranspriteMaterial&&) = delete;
	TranspriteMaterial& operator=(TranspriteMaterial&&) = delete;

	void UpdateShaderVariables(const real::DrawableComponent* mesh, uint32_t reference) override;

protected:
	void CreatePipeline() override;
	void CreateDescriptorSetLayout() override;
	void CreateDescriptorSets(uint32_t reference, VkDescriptorSetLayout layout) override;
};

#endif // TRANSPRITEMATERIAL_H