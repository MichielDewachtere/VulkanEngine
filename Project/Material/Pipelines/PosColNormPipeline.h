#ifndef POSCOLNORMPIPELINE_H
#define POSCOLNORMPIPELINE_H

#include "PosNormBase.h"

class PosColNormPipeline : public PosNormBase<PosColNorm>
{
public:
	PosColNormPipeline() = default;
	virtual ~PosColNormPipeline() override = default;

	virtual void CleanUp(VkDevice device) override;

	virtual void CreatePipeline(const VulkanContext& vulkan) override;
private:
	void CreateDescriptorSetLayout(const VkDevice& device);
};

#endif // POSCOLNORMPIPELINE_H