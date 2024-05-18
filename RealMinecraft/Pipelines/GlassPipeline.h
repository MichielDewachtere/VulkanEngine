#ifndef GLASSPIPELINE_H
#define GLASSPIPELINE_H

#include "TransparencyPipeline.h"

class GlassPipeline final : public TransparencyPipeline
{
public:
	GlassPipeline() = default;
	virtual ~GlassPipeline() override = default;

	virtual void CleanUp(VkDevice device) override;

	virtual void CreatePipeline(const real::VulkanContext& vulkan) override;

private:
	virtual void CreateDescriptorSetLayout(const VkDevice& device) override;
};

#endif // GLASSPIPELINE_H