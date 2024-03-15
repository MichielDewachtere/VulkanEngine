#ifndef DEFAULTPIPELINE_H
#define DEFAULTPIPELINE_H

#include "Pipeline.h"

class DefaultPipeline final : public Pipeline
{
public:
	DefaultPipeline() = default;
	virtual ~DefaultPipeline() override = default;

	virtual void CreatePipeline(const VkDevice& device, const RenderPass*) override;
};

#endif // DEFAULTPIPELINE_H