#ifndef COMMANDBUFFER_H
#define COMMANDBUFFER_H

#include <vulkan/vulkan_core.h>

#include "Util/Structs.h"

class CommandBuffer final
{
public:
	explicit CommandBuffer(const GameContext& context);
	~CommandBuffer() = default;

	CommandBuffer(const CommandBuffer&) = delete;
	CommandBuffer& operator=(const CommandBuffer&) = delete;
	CommandBuffer(CommandBuffer&&) = delete;
	CommandBuffer& operator=(CommandBuffer&&) = delete;

	void CreateCommandBufferPool(const GameContext& context);
	void CreateCommandBuffer(const VulkanContext& context);

	VkCommandBuffer GetCommandBuffer() const { return m_CommandBuffer; }

	void CleanUp(VkDevice device);

private:
	VkCommandPool m_CommandPool{};
	VkCommandBuffer m_CommandBuffer{};
};

#endif // COMMANDBUFFER_H