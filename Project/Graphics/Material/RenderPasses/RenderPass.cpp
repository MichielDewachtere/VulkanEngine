#include "RenderPass.h"

void RenderPass::CleanUp(VkDevice device)
{
	vkDestroyRenderPass(device, m_RenderPass, nullptr);
}
