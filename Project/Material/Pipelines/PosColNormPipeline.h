#ifndef POSCOLNORMPIPELINE_H
#define POSCOLNORMPIPELINE_H

#include "Pipeline.h"

class PosColNormPipeline final : public Pipeline
{
public:
	PosColNormPipeline() = default;
	virtual ~PosColNormPipeline() override = default;

	virtual void CleanUp(VkDevice device) override;

	virtual void CreatePipeline(const VulkanContext& device) override;
private:
	void CreateDescriptorSetLayout(const VkDevice& device);
};

#endif // POSCOLNORMPIPELINE_H