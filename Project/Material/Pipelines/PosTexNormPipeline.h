#ifndef POSTEXNORMPIPELINE_H
#define POSTEXNORMPIPELINE_H

#include "PosNormBase.h"

class PosTexNormPipeline final : public PosNormBase<PosTexNorm>
{
public:
	PosTexNormPipeline() = default;
	virtual ~PosTexNormPipeline() override = default;

	virtual void CleanUp(VkDevice device) override;

	virtual void CreatePipeline(const VulkanContext& vulkan) override;
private:
	void CreateDescriptorSetLayout(const VkDevice& device);
};

#endif // POSCOLTEXPIPELINE_H