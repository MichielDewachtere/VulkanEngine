#ifndef BASEMATERIAL_H
#define BASEMATERIAL_H

#include <cstdint>
#include <map>

#include <vulkan/vulkan.h>

#include "Util/VulkanUtil.h"

namespace real
{
	class BaseMaterial
	{
	public:
		BaseMaterial() = default;
		virtual ~BaseMaterial() = default;

		BaseMaterial(const BaseMaterial&) = delete;
		BaseMaterial& operator=(const BaseMaterial&) = delete;
		BaseMaterial(BaseMaterial&&) = delete;
		BaseMaterial& operator=(BaseMaterial&&) = delete;

		virtual void Init() = 0;
		virtual void CleanUp() = 0;

		virtual void CleanUpUbo(uint32_t reference) = 0;

		void Bind(VkCommandBuffer buffer, uint32_t reference);


	protected:
		VkPipeline m_Pipeline{ nullptr };
		VkPipelineLayout m_PipelineLayout{ nullptr };

		std::map<uint32_t, std::array<VkBuffer, MAX_FRAMES_IN_FLIGHT>> m_UniformBuffers{};
		std::map<uint32_t, std::array<VmaAllocation, MAX_FRAMES_IN_FLIGHT>> m_UniformBufferAllocations{};
		std::map<uint32_t, std::array<void*, MAX_FRAMES_IN_FLIGHT>> m_UniformBuffersMapped{};

		VkDescriptorSetLayout m_DescriptorSetLayout{ nullptr };
		std::map<uint32_t, std::array<VkDescriptorSet, MAX_FRAMES_IN_FLIGHT>> m_DescriptorSets{};
	};
}

#endif // BASEMATERIAL_H