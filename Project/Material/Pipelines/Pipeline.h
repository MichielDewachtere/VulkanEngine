#ifndef PIPELINE_H
#define PIPELINE_H

#include "Util/VulkanUtil.h"

class RenderPass;

class Pipeline
{
public:
	Pipeline() = default;
	virtual ~Pipeline() = default;

	Pipeline(const Pipeline&) = delete;
	Pipeline& operator=(const Pipeline&) = delete;
	Pipeline(Pipeline&&) = delete;
	Pipeline& operator=(Pipeline&&) = delete;

	virtual void CreatePipeline(const VkDevice& device, const RenderPass* pRenderPass) = 0;
	VkPipeline GetPipeline() const { return m_Pipeline; }

	bool IsCreated() const { return m_IsCreated; }

	void CleanUp(VkDevice device);

protected:
	bool m_IsCreated{ false };
	VkPipeline m_Pipeline;
	VkPipelineLayout m_PipelineLayout;
};

#endif // PIPELINE_H