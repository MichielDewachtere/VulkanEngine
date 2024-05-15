#ifndef COMMANDPOOL_H
#define COMMANDPOOL_H

#include <memory>
#include <vulkan/vulkan_core.h>
#include <real_core/Singleton.h>

#include "CommandBuffers/CommandBuffer.h"

struct GameContext;

namespace real
{
	class CommandPool final : public real::Singleton<CommandPool>
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

		VkCommandPool m_CommandPool{ nullptr };
		std::unique_ptr<CommandBuffer> m_pCommandBuffer{ nullptr };
	};
}

#endif // COMMANDPOOL_H