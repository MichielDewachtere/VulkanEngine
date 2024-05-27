#ifndef WATERMATERIAL_H
#define WATERMATERIAL_H

#include "Material/Material.h"
#include "Util/GameStructs.h"

class WaterMaterial final : public real::Material<WVP_Time>
{
public:
	explicit WaterMaterial();
	virtual ~WaterMaterial() override = default;

	WaterMaterial(const WaterMaterial&) = delete;
	WaterMaterial& operator=(const WaterMaterial&) = delete;
	WaterMaterial(WaterMaterial&&) = delete;
	WaterMaterial& operator=(WaterMaterial&&) = delete;

	void UpdateShaderVariables(const real::DrawableComponent* mesh, uint32_t reference) override;

protected:
	void CreatePipeline() override;
	void CreateDescriptorSetLayout() override;
	void CreateDescriptorSets(uint32_t reference, VkDescriptorSetLayout layout) override;

private:
	float m_MaxTime{ 1.6f }, m_AmountOfSprites{ 32.f }, m_Interval{ 0.05f };
	static inline uint32_t m_TimerId{ 0 };
};

#endif // WATERMATERIAL_H