#include "Pipeline.h"

void Pipeline::CleanUp(VkDevice device)
{
	vkDestroyPipeline(device, m_Pipeline, nullptr);
	vkDestroyPipelineLayout(device, m_PipelineLayout, nullptr);
}
