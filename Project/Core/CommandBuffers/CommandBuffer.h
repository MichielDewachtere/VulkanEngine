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
	VkCommandBuffer GetCommandBuffer(uint32_t currentFrame) const { return m_CommandBuffers[currentFrame]; }

	void StartRecording(uint32_t currentFrame) const;
	static void StartRecording(VkCommandBuffer buffer);
	void StopRecording(uint32_t currentFrame) const;
	static void StopRecording(VkCommandBuffer buffer);

private:
	std::vector<VkCommandBuffer> m_CommandBuffers{};
};

#endif // COMMANDBUFFER_H