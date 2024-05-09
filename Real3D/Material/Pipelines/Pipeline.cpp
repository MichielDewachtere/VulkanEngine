#include "Pipeline.h"

void real::Pipeline::CleanUp(VkDevice device)
{
	vkDestroyDescriptorSetLayout(device, m_DescriptorSetLayout, nullptr);

	vkDestroyPipeline(device, m_Pipeline, nullptr);
	vkDestroyPipelineLayout(device, m_PipelineLayout, nullptr);
}
