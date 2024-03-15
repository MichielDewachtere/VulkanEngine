#ifndef COMMANDBUFFER_H
#define COMMANDBUFFER_H

#include <vulkan/vulkan_core.h>

#include "Util/Structs.h"

class CommandBuffer final
{
public:
	explicit CommandBuffer() = default;
	~CommandBuffer() = default;

	CommandBuffer(const CommandBuffer&) = delete;
	CommandBuffer& operator=(const CommandBuffer&) = delete;
	CommandBuffer(CommandBuffer&&) = delete;
	CommandBuffer& operator=(CommandBuffer&&) = delete;

	void CreateCommandBuffer(const VulkanContext& context);
	VkCommandBuffer GetCommandBuffer() const { return m_CommandBuffer; }

private:
	VkCommandBuffer m_CommandBuffer{};
};

#endif // COMMANDBUFFER_H