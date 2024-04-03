#include "CommandPool.h"

#include <stdexcept>

#include "Engine.h"

void CommandPool::Init(const GameContext& context)
{
	const QueueFamilyIndices queueFamilyIndices = Engine::FindQueueFamilies(context);

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

	if (vkCreateCommandPool(context.vulkanContext.device, &poolInfo, nullptr, &m_CommandPool) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create command pool!");
	}

	m_pCommandBuffer = std::make_unique<CommandBuffer>();
	m_pCommandBuffer->CreateCommandBuffer(context.vulkanContext);
}

void CommandPool::CleanUp(const GameContext& context) const
{
	vkDestroyCommandPool(context.vulkanContext.device, m_CommandPool, nullptr);
}

VkCommandBuffer CommandPool::GetCommandBuffer(uint32_t frame) const
{
	return m_pCommandBuffer->GetCommandBuffer(frame);
}