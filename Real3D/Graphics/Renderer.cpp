#include "Renderer.h"

#include <array>
#include <vulkan/vulkan_core.h>

#include "RealEngine.h"
#include "RenderPass.h"
#include "Core/SwapChain.h"
#include "Core/CommandPool.h"
#include "Core/CommandBuffers/CommandBuffer.h"
#include "Core/DepthBuffer/DepthBufferManager.h"
#include "Material/MaterialManager.h"
#include "real_core/SceneManager.h"

void real::Renderer::Init(GameContext& context)
{
	// Create Queues
	const QueueFamilyIndices indices = FindQueueFamilies(context.vulkanContext.physicalDevice, context.vulkanContext.surface);

	vkGetDeviceQueue(context.vulkanContext.device, indices.graphicsFamily.value(), 0, &m_GraphicsQueue);
	vkGetDeviceQueue(context.vulkanContext.device, indices.presentFamily.value(), 0, &m_PresentQueue);

	// Create Swap chain
	m_pSwapChain = new SwapChain();
	m_pSwapChain->CreateSwapChain(context);

	// Create Image Views
	m_pSwapChain->CreateSwapChainImages(context);

	// Create Render Pass
	const auto renderPass = new RenderPass();
	renderPass->Create(context, m_pSwapChain->GetFormat());
	context.vulkanContext.renderPass = renderPass->GetRenderPass();

	// Create Depth Buffer
	DepthBufferManager::GetInstance().AddDepthBuffer(context);

	// Create Frame Buffers
	CreateFrameBuffers(context);

	// Create Sync Objects
	CreateSyncObjects(context);
}

void real::Renderer::CleanUp(const GameContext& context) const
{
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
	{
		vkDestroySemaphore(context.vulkanContext.device, m_RenderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(context.vulkanContext.device, m_ImageAvailableSemaphores[i], nullptr);
		vkDestroyFence(context.vulkanContext.device, m_InFlightFences[i], nullptr);
	}

	for (const auto frameBuffer : m_SwapChainFrameBuffers) 
	{
		vkDestroyFramebuffer(context.vulkanContext.device, frameBuffer, nullptr);
	}

	m_pSwapChain->CleanUp(context);
}

void real::Renderer::CreateFrameBuffers(const GameContext& context)
{
	const auto swapChainImageViews = m_pSwapChain->GetImageViews();
	const auto swapChainExtent = m_pSwapChain->GetExtent();

	m_SwapChainFrameBuffers.resize(swapChainImageViews.size());
	for (size_t i = 0; i < swapChainImageViews.size(); ++i)
	{
		std::array attachments{
			swapChainImageViews[i],
			DepthBufferManager::GetInstance().GetDepthBuffer(0)->GetImageView()
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = context.vulkanContext.renderPass;
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = swapChainExtent.width;
		framebufferInfo.height = swapChainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(context.vulkanContext.device, &framebufferInfo, nullptr, &m_SwapChainFrameBuffers[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}

void real::Renderer::CreateSyncObjects(const GameContext& context)
{
	m_ImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	m_RenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	m_InFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		if (vkCreateSemaphore(context.vulkanContext.device, &semaphoreInfo, nullptr, &m_ImageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(context.vulkanContext.device, &semaphoreInfo, nullptr, &m_RenderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(context.vulkanContext.device, &fenceInfo, nullptr, &m_InFlightFences[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create synchronization objects for a frame!");
		}
	}
}

void real::Renderer::Draw(const GameContext& context)
{
	vkWaitForFences(context.vulkanContext.device, 1, &m_InFlightFences[m_CurrentFrame], VK_TRUE, UINT64_MAX);
	vkResetFences(context.vulkanContext.device, 1, &m_InFlightFences[m_CurrentFrame]);

	uint32_t imageIndex;
	vkAcquireNextImageKHR(context.vulkanContext.device, m_pSwapChain->GetSwapChain(), UINT64_MAX,
	                      m_ImageAvailableSemaphores[m_CurrentFrame], VK_NULL_HANDLE, &imageIndex);

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = context.vulkanContext.renderPass;
	renderPassInfo.framebuffer = m_SwapChainFrameBuffers[imageIndex];
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = m_pSwapChain->GetExtent();

	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = { {0.392f, 0.584f, 0.929f, 1.0f} };
	clearValues[1].depthStencil = { 1.0f, 0 };
	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	const auto commandBuffer = CommandPool::GetInstance().GetCommandBuffer()->SetCommandBufferActive(m_CurrentFrame);
	CommandBuffer::StartRecording(commandBuffer);
	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)m_pSwapChain->GetExtent().width;
	viewport.height = (float)m_pSwapChain->GetExtent().height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = m_pSwapChain->GetExtent();
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	//TODO: Call SceneManager::Render instead

	SceneManager::GetInstance().Render();

	//for (const auto& pMaterial : MaterialManager::GetInstance().GetMaterials())
	//{
	//	if (pMaterial->IsActive())
	//		pMaterial->DrawFrame(m_CurrentFrame, m_pSwapChain->GetExtent());
	//}

	vkCmdEndRenderPass(commandBuffer);
	CommandBuffer::StopRecording(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { m_ImageAvailableSemaphores[m_CurrentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	VkSemaphore signalSemaphores[] = { m_RenderFinishedSemaphores[m_CurrentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, m_InFlightFences[m_CurrentFrame]) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	const VkSwapchainKHR swapChains[] = { m_pSwapChain->GetSwapChain() };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;

	presentInfo.pImageIndices = &imageIndex;

	vkQueuePresentKHR(m_PresentQueue, &presentInfo);

	m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}
