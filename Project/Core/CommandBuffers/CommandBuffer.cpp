#include "CommandBuffer.h"

#include <stdexcept>

#include "Engine.h"
#include "Core/CommandPool.h"
#include "Util/VulkanUtil.h"
#include "Graphics/Renderer.h"

void CommandBuffer::CreateCommandBuffer(const VulkanContext& context)
{
	m_CommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = CommandPool::GetInstance().GetCommandPool();
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)m_CommandBuffers.size();

	if (vkAllocateCommandBuffers(context.device, &allocInfo, m_CommandBuffers.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate command buffers!");
	}
}

void CommandBuffer::StartRecording(uint32_t currentFrame) const
{
	StartRecording(m_CommandBuffers[currentFrame]);
}

void CommandBuffer::StartRecording(VkCommandBuffer buffer)
{
	vkResetCommandBuffer(buffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0; // Optional
	beginInfo.pInheritanceInfo = nullptr; // Optional

	if (vkBeginCommandBuffer(buffer, &beginInfo) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to begin recording command buffer!");
	}
}

void CommandBuffer::StopRecording(uint32_t currentFrame) const
{
	StopRecording(m_CommandBuffers[currentFrame]);
}

void CommandBuffer::StopRecording(VkCommandBuffer buffer)
{
	if (vkEndCommandBuffer(buffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to record command buffer!");
	}
}

VkCommandBuffer CommandBuffer::StartSingleTimeCommands(const GameContext& context)
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = CommandPool::GetInstance().GetCommandPool();
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(context.vulkanContext.device, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	return commandBuffer;
}

void CommandBuffer::StopSingleTimeCommands(const GameContext& context, VkCommandBuffer commandBuffer)
{
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(Renderer::GetInstance().GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(Renderer::GetInstance().GetGraphicsQueue());

	vkFreeCommandBuffers(context.vulkanContext.device, CommandPool::GetInstance().GetCommandPool(), 1, &commandBuffer);
}
