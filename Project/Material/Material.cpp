#include "Material.h"

#include <stdexcept>

#include "CommandBuffers/CommandBuffer.h"
#include "Core/SwapChain.h"
#include "Mesh/Mesh.h"
#include "Pipelines/Pipeline.h"
#include "RenderPasses/RenderPass.h"

void Material::CleanUp(const GameContext& context)
{
	m_pPipeline->CleanUp(context.vulkanContext.device);
	m_pRenderPass->CleanUp(context.vulkanContext.device);
}

void Material::RecordCommandBuffer(const SwapChain& swapChain, const VkFramebuffer& frameBuffer)
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0; // Optional
	beginInfo.pInheritanceInfo = nullptr; // Optional

	if (vkBeginCommandBuffer(m_pCommandBuffer->GetCommandBuffer(), &beginInfo) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	DrawFrame(swapChain, frameBuffer);

	if (vkEndCommandBuffer(m_pCommandBuffer->GetCommandBuffer()) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to record command buffer!");
	}
}

void Material::DrawFrame(const SwapChain& swapChain, const VkFramebuffer& frameBuffer)
{
	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = m_pRenderPass->GetRenderPass();
	renderPassInfo.framebuffer = frameBuffer;
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = swapChain.GetExtent();

	VkClearValue clearColor = { {{173.f / 255.f, 216.f / 255.f, 230.f / 255.f, 1.0f}} };
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(m_pCommandBuffer->GetCommandBuffer(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(m_pCommandBuffer->GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_pPipeline->GetPipeline());

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)swapChain.GetExtent().width;
	viewport.height = (float)swapChain.GetExtent().height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(m_pCommandBuffer->GetCommandBuffer(), 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = swapChain.GetExtent();
	vkCmdSetScissor(m_pCommandBuffer->GetCommandBuffer(), 0, 1, &scissor);

	// DrawScene();
	for (const auto & m : m_pMeshes)
	{
		m->Draw(m_pCommandBuffer->GetCommandBuffer());
	}

	//vkCmdDraw(m_pCommandBuffer->GetCommandBuffer(), 3, 1, 0, 0);
	vkCmdEndRenderPass(m_pCommandBuffer->GetCommandBuffer());
}
