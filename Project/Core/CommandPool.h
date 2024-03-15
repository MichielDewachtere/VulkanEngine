#ifndef COMMANDPOOL_H
#define COMMANDPOOL_H

#include <memory>
#include <vulkan/vulkan_core.h>

#include "Util/Singleton.h"

struct GameContext;
class CommandBuffer;

//TODO: This could use the service locator pattern or singleton manager?
class CommandPool final : public Singleton<CommandPool>
{
public:
	virtual ~CommandPool() override = default;

	void Init(const GameContext& context);  // NOLINT(clang-diagnostic-overloaded-virtual)
	void CleanUp(const GameContext& context);

	VkCommandPool GetCommandPool() const { return m_CommandPool; }
	//VkCommandPool GetCommandPool() const { return *m_pCommandPool; }
	//void FreeCommandBuffer();

private:
	friend class Singleton<CommandPool>;
	CommandPool() = default;

	VkCommandPool m_CommandPool;
	//std::unique_ptr<VkCommandPool> m_pCommandPool;
};

#endif // COMMANDPOOL_H