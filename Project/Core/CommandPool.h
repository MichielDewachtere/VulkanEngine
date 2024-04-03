#ifndef COMMANDPOOL_H
#define COMMANDPOOL_H

#include <memory>
#include <vulkan/vulkan_core.h>

#include "Util/Singleton.h"
#include "CommandBuffers/CommandBuffer.h"

struct GameContext;

class CommandPool final : public Singleton<CommandPool>
{
public:
	virtual ~CommandPool() override = default;

	void Init(const GameContext& context);  // NOLINT(clang-diagnostic-overloaded-virtual)
	void CleanUp(const GameContext& context) const;

	VkCommandPool GetCommandPool() const { return m_CommandPool; }

	VkCommandBuffer GetCommandBuffer(uint32_t frame) const;

private:
	friend class Singleton<CommandPool>;
	CommandPool() = default;

	VkCommandPool m_CommandPool;
	std::unique_ptr<CommandBuffer> m_pCommandBuffer;
};

#endif // COMMANDPOOL_H