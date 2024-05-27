#ifndef GUIMATERIAL_H
#define GUIMATERIAL_H

#include "Material/Material.h"
#include "Util/GameStructs.h"

class GuiMaterial final : public real::Material<WorldMatrix>
{
public:
	explicit GuiMaterial() = default;
	virtual ~GuiMaterial() override = default;

	GuiMaterial(const GuiMaterial&) = delete;
	GuiMaterial& operator=(const GuiMaterial&) = delete;
	GuiMaterial(GuiMaterial&&) = delete;
	GuiMaterial& operator=(GuiMaterial&&) = delete;

	void UpdateShaderVariables(const real::DrawableComponent* mesh, uint32_t reference) override;

protected:
	void CreatePipeline() override;
	void CreateDescriptorSetLayout() override;
	void CreateDescriptorSets(uint32_t reference, VkDescriptorSetLayout layout) override;
};

#endif // GUIMATERIAL_H