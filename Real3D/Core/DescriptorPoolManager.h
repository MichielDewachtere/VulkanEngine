#ifndef DESCRIPTORPOOLMANAGER_H
#define DESCRIPTORPOOLMANAGER_H

#include <cstdint>
#include <vector>

#include <vulkan/vulkan.h>

#include <real_core/Singleton.h>

namespace real
{
	class DescriptorPoolManager final : public Singleton<DescriptorPoolManager>
	{
	public:
		virtual ~DescriptorPoolManager() override = default;

		DescriptorPoolManager(const DescriptorPoolManager&) = delete;
		DescriptorPoolManager& operator=(const DescriptorPoolManager&) = delete;
		DescriptorPoolManager(DescriptorPoolManager&&) = delete;
		DescriptorPoolManager& operator=(DescriptorPoolManager&&) = delete;

		void CleanUp();

		VkDescriptorSet AllocateDescriptorSet(VkDescriptorSetLayout layout);
		VkDescriptorPool GetCurrentPool() const { return m_CurrentPool; }

	private:
		friend class Singleton<DescriptorPoolManager>;
		explicit DescriptorPoolManager();

		std::vector<VkDescriptorPool> m_DescriptorPools;
		VkDescriptorPool m_CurrentPool;
		uint32_t m_CurrentPoolSize;
		const uint32_t m_InitialPoolSize = 100;
		const float m_GrowthFactor = 1.5f;

		static VkDescriptorPool CreateDescriptorPool(uint32_t size);
		void AllocateNewPool();
	};
}

#endif // DESCRIPTORPOOLMANAGER_H