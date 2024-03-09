#include "Renderer.h"

#include <vulkan/vulkan_core.h>
#include "Engine.h"
#include "Core/SwapChain.h"
#include "Material/CommandBuffers/CommandBuffer.h"
#include "Material/RenderPasses/RenderPass.h"

void Renderer::Init(const GameContext& context)
{
	// Create Queues
	QueueFamilyIndices indices = Engine::FindQueueFamilies(context);

	vkGetDeviceQueue(context.vulkanContext.device, indices.graphicsFamily.value(), 0, &m_GraphicsQueue);
	vkGetDeviceQueue(context.vulkanContext.device, indices.presentFamily.value(), 0, &m_PresentQueue);

	// Create Swap chain
	m_pSwapChain = new SwapChain();
	m_pSwapChain->CreateSwapChain(context);

	// Create Image Views
	m_pSwapChain->CreateSwapChainImages(context);
}

void Renderer::CleanUp(const GameContext& context)
{
	vkDestroySemaphore(context.vulkanContext.device, m_RenderFinishedSemaphore, nullptr);
	vkDestroySemaphore(context.vulkanContext.device, m_ImageAvailableSemaphore, nullptr);
	vkDestroyFence(context.vulkanContext.device, m_InFlightFence, nullptr);

	m_pMaterial->GetCommandBuffer()->CleanUp(context.vulkanContext.device);
	for (auto framebuffer : m_SwapChainFrameBuffers) 
	{
		vkDestroyFramebuffer(context.vulkanContext.device, framebuffer, nullptr);
	}

	m_pMaterial->GetPipeline()->CleanUp(context.vulkanContext.device);
	m_pMaterial->GetRenderPass()->CleanUp(context.vulkanContext.device);

	m_pSwapChain->CleanUp(context);
}

void Renderer::CreateFrameBuffers(const GameContext& context)
{
	const auto swapChainImageViews = m_pSwapChain->GetImageViews();
	const auto swapChainExtent = m_pSwapChain->GetExtent();

	m_SwapChainFrameBuffers.resize(swapChainImageViews.size());
	for (size_t i = 0; i < swapChainImageViews.size(); i++)
	{
		VkImageView attachments[] =
		{
			swapChainImageViews[i]
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = m_pMaterial->GetRenderPass()->GetRenderPass();
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = swapChainExtent.width;
		framebufferInfo.height = swapChainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(context.vulkanContext.device, &framebufferInfo, nullptr, &m_SwapChainFrameBuffers[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}

void Renderer::CreateSyncObjects(const GameContext& context)
{
	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	if (vkCreateSemaphore(context.vulkanContext.device, &semaphoreInfo, nullptr, &m_ImageAvailableSemaphore) != VK_SUCCESS ||
		vkCreateSemaphore(context.vulkanContext.device, &semaphoreInfo, nullptr, &m_RenderFinishedSemaphore) != VK_SUCCESS ||
		vkCreateFence(context.vulkanContext.device, &fenceInfo, nullptr, &m_InFlightFence) != VK_SUCCESS) {
		throw std::runtime_error("failed to create synchronization objects for a frame!");
	}
}

void Renderer::AddMaterial(Material* pMaterial)
{
	m_pMaterial = pMaterial;
}

void Renderer::Draw(const GameContext& context)
{
	vkWaitForFences(context.vulkanContext.device, 1, &m_InFlightFence, VK_TRUE, UINT64_MAX);
	vkResetFences(context.vulkanContext.device, 1, &m_InFlightFence);

	uint32_t imageIndex;
	vkAcquireNextImageKHR(context.vulkanContext.device, m_pSwapChain->GetSwapChain(), UINT64_MAX, m_ImageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

	auto commandBuffer = m_pMaterial->GetCommandBuffer()->GetCommandBuffer();

	vkResetCommandBuffer(commandBuffer, /*VkCommandBufferResetFlagBits*/ 0);
	m_pMaterial->RecordCommandBuffer(*m_pSwapChain, m_SwapChainFrameBuffers[imageIndex]);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { m_ImageAvailableSemaphore };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	VkSemaphore signalSemaphores[] = { m_RenderFinishedSemaphore };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, m_InFlightFence) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { m_pSwapChain->GetSwapChain() };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;

	presentInfo.pImageIndices = &imageIndex;

	vkQueuePresentKHR(m_PresentQueue, &presentInfo);
}
