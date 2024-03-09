#include "CommandBuffer.h"

#include <stdexcept>

#include "Engine.h"

CommandBuffer::CommandBuffer(const GameContext& context)
{
	CreateCommandBufferPool(context);
	CreateCommandBuffer(context.vulkanContext);
}

void CommandBuffer::CreateCommandBufferPool(const GameContext& context)
{
	QueueFamilyIndices queueFamilyIndices = Engine::FindQueueFamilies(context);

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

	if (vkCreateCommandPool(context.vulkanContext.device, &poolInfo, nullptr, &m_CommandPool) != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to create command pool!");
	}
}

void CommandBuffer::CreateCommandBuffer(const VulkanContext& context)
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = m_CommandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	if (vkAllocateCommandBuffers(context.device, &allocInfo, &m_CommandBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate command buffers!");
	}

}

void CommandBuffer::CleanUp(VkDevice device)
{
	vkDestroyCommandPool(device, m_CommandPool, nullptr);
}
