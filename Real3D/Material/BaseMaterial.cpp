#include "BaseMaterial.h"

#include "Graphics/Renderer.h"

void real::BaseMaterial::Bind(VkCommandBuffer buffer, uint32_t reference)
{
	vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);
	vkCmdBindDescriptorSets(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1,
		&m_DescriptorSets[reference][real::Renderer::GetInstance().GetCurrentFrame()], 0, nullptr);
}
