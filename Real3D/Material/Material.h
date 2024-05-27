#ifndef MATERIALNEW_H
#define MATERIALNEW_H

#include <map>

#include "Util/vk_mem_alloc.h"
#include "Graphics/Renderer.h"
#include "Util/VulkanUtil.h"
#include "Core/CommandPool.h"

#include "BaseMaterial.h"
#include "RealEngine.h"

namespace real
{
	class DrawableComponent;

	template <typename Ubo>
	class Material : public BaseMaterial
	{
	public:
		explicit Material() = default;
		virtual ~Material() override = default;

		Material(const Material&) = delete;
		Material& operator=(const Material&) = delete;
		Material(Material&&) = delete;
		Material& operator=(Material&&) = delete;

		virtual void Init() override;
		virtual void CleanUp() override;
		virtual void CleanUpUbo(uint32_t reference) override;

		uint32_t AddReference(); // To keep track of amount of meshes?

		virtual void UpdateShaderVariables(const DrawableComponent* mesh, uint32_t reference) = 0;
		void UpdateUniformBuffer(uint32_t reference, Ubo ubo);

	protected:
		virtual void CreatePipeline() = 0;
		virtual void CreateDescriptorSetLayout() = 0;
		virtual void CreateDescriptorSets(uint32_t reference, VkDescriptorSetLayout layout) = 0;

	private:
		inline static uint32_t m_AmountOfMeshes{ 0 };

		void CreateUniformBuffers(uint32_t reference);
	};

	template <typename Ubo>
	void Material<Ubo>::Init()
	{
		CreateDescriptorSetLayout();
		CreatePipeline();
	}

	template <typename Ubo>
	void Material<Ubo>::CleanUp()
	{
		const auto context = RealEngine::GetGameContext();

		for (const auto& [reference, buffers] : m_UniformBuffers)
		{
			for (size_t j = 0; j < buffers.size(); ++j)
			{
				vmaUnmapMemory(context.vulkanContext.allocator, m_UniformBufferAllocations[reference][j]);
				vmaDestroyBuffer(context.vulkanContext.allocator, m_UniformBuffers[reference][j], m_UniformBufferAllocations[reference][j]);
			}
		}

		vkDestroyDescriptorSetLayout(context.vulkanContext.device, m_DescriptorSetLayout, nullptr);

		vkDestroyPipeline(context.vulkanContext.device, m_Pipeline, nullptr);
		vkDestroyPipelineLayout(context.vulkanContext.device, m_PipelineLayout, nullptr);
	}

	template <typename Ubo>
	void Material<Ubo>::CleanUpUbo(uint32_t reference)
	{
		const auto context = RealEngine::GetGameContext();

		for (size_t i = 0; i < m_UniformBuffers[reference].size(); ++i)
		{
			vmaUnmapMemory(context.vulkanContext.allocator, m_UniformBufferAllocations[reference][i]);
			vmaDestroyBuffer(context.vulkanContext.allocator, m_UniformBuffers[reference][i], m_UniformBufferAllocations[reference][i]);
		}

		m_UniformBuffers.erase(reference);
	}

	template <typename Ubo>
	uint32_t Material<Ubo>::AddReference()
	{
		++m_AmountOfMeshes;

		CreateUniformBuffers(m_AmountOfMeshes);
		CreateDescriptorSets(m_AmountOfMeshes, m_DescriptorSetLayout);

		return m_AmountOfMeshes;

	}

	template <typename Ubo>
	void Material<Ubo>::UpdateUniformBuffer(uint32_t reference, Ubo ubo)
	{
		memcpy(m_UniformBuffersMapped[reference][Renderer::GetInstance().GetCurrentFrame()], &ubo, sizeof(Ubo));

		const auto buffer = CommandPool::GetInstance().GetActiveCommandBuffer();
		const auto frame = Renderer::GetInstance().GetCurrentFrame();
		vkCmdBindDescriptorSets(buffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_PipelineLayout,
			0,
			1,
			&m_DescriptorSets[reference][frame],
			0,
			nullptr
		);
	}

	template <typename Ubo>
	void Material<Ubo>::CreateUniformBuffers(uint32_t reference)
	{
		VkDeviceSize bufferSize = sizeof(Ubo);

		const auto context = RealEngine::GetGameContext();

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			CreateBuffer(context, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_UniformBuffers[reference][i],
				m_UniformBufferAllocations[reference][i]);

			vmaMapMemory(context.vulkanContext.allocator, m_UniformBufferAllocations[reference][i], &m_UniformBuffersMapped[reference][i]);
		}
	}
}

#endif // MATERIALNEW_H