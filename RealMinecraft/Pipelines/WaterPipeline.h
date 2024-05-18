#ifndef	WATERPIPELINE_H
#define	WATERPIPELINE_H

#include "TransparencyPipeline.h"

class WaterPipeline final : public TransparencyPipeline
{
public:
	WaterPipeline() = default;
	virtual ~WaterPipeline() override = default;

	virtual void CleanUp(VkDevice device) override;

	virtual void CreatePipeline(const real::VulkanContext& vulkan) override;

private:
	virtual void CreateDescriptorSetLayout(const VkDevice& device) override;
};

#endif // WATERPIPELINE_H