#ifndef MESH_H
#define MESH_H

#include <iomanip>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <vulkan/vulkan_core.h>

#include <real_core/Component.h>
#include <real_core/GameObject.h>

#include "Misc/CameraManager.h"
#include "Misc/Camera.h"
#include "Core/CommandPool.h"
#include "Content/ContentManager.h"
#include "Util/Concepts.h"
#include "Util/Structs.h"
#include "Util/VulkanUtil.h"
#include "RealEngine.h"

namespace real
{
	struct MeshInfo
	{
		uint32_t vertexCapacity = 0, indexCapacity = 0;
		Texture2D* texture = nullptr;
		bool usesUbo = false;
	};

	template <vertex_type V>
	class Mesh : public real::Component
	{
	public:
		explicit Mesh(real::GameObject* pOwner, MeshInfo info)
			: Component(pOwner)
			, m_Info(info) {}

		virtual ~Mesh() override = default;

		Mesh(const Mesh&) = delete;
		Mesh& operator=(const Mesh&) = delete;
		Mesh(Mesh&&) = delete;
		Mesh& operator=(Mesh&&) = delete;

		virtual void Init(const GameContext& context)
		{
			CreateVertexBuffer(context);
		}

		virtual void Kill() override
		{
			const auto context = RealEngine::GetGameContext();

			for (size_t i = 0; i < m_UniformBuffers.size(); i++)
			{
				vkDestroyBuffer(context.vulkanContext.device, m_UniformBuffers[i], nullptr);
				vkFreeMemory(context.vulkanContext.device, m_UniformBuffersMemory[i], nullptr);
			}

			vkDestroyBuffer(context.vulkanContext.device, m_VertexBuffer, nullptr);
			vkFreeMemory(context.vulkanContext.device, m_VertexBufferMemory, nullptr);

			vkDestroyBuffer(context.vulkanContext.device, m_StagingBuffer, nullptr);
			vkFreeMemory(context.vulkanContext.device, m_StagingBufferMemory, nullptr);

			if (m_Info.usesUbo)
				vkDestroyDescriptorPool(context.vulkanContext.device, m_DescriptorPool, nullptr);
		}

		void CreateDescriptor(const GameContext& context, VkDescriptorSetLayout layout)
		{
			if (m_Info.usesUbo == false)
				return;

			CreateUniformBuffers(context);
			CreateDescriptorPool(context);
			CreateDescriptorSets(context, layout);
		}

		void UpdateUbo(uint32_t currentFrame, VkCommandBuffer buffer, VkPipelineLayout layout) const
		{
			if (m_Info.usesUbo == false)
				return;

			UniformBufferObject ubo{};
			ubo.model = GetOwner()->GetTransform()->GetWorldMatrix();
			ubo.proj = CameraManager::GetInstance().GetActiveCamera()->GetProjection();
			ubo.proj[1][1] *= -1;
			ubo.view = CameraManager::GetInstance().GetActiveCamera()->GetView();

			memcpy(m_UniformBuffersMapped[currentFrame], &ubo, sizeof(UniformBufferObject));

			vkCmdBindDescriptorSets(buffer,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				layout,
				0,
				1,
				&m_DescriptorSets[currentFrame],
				0,
				nullptr
			);
		}
		virtual void Draw(VkCommandBuffer commandBuffer)
		{
			const VkBuffer vertexBuffers[] = { m_VertexBuffer };
			constexpr VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

			vkCmdDraw(commandBuffer, static_cast<uint32_t>(m_Vertices.size()), 1, 0, 0);
		}

		void AddVertex(V vertex)
		{
			m_Vertices.push_back(vertex);
		}
		void AddVertices(const std::vector<V>& vertices)
		{
			m_Vertices.insert(m_Vertices.end(), vertices.begin(), vertices.end());
		}

	protected:
		MeshInfo m_Info;
		std::vector<V> m_Vertices{};

		// Vertex buffer
		VkBuffer m_VertexBuffer{ nullptr }, m_StagingBuffer{ nullptr };
		VkDeviceMemory m_VertexBufferMemory{ nullptr }, m_StagingBufferMemory{ nullptr };

		// Uniform (Constant) buffer
		std::vector<VkBuffer> m_UniformBuffers{ nullptr };
		std::vector<VkDeviceMemory> m_UniformBuffersMemory{ nullptr };
		std::vector<void*> m_UniformBuffersMapped{ nullptr };

		// TODO: Ask => one per pipeline / one per mesh??
		// They use same UBO/Samplers but different data
		VkDescriptorPool m_DescriptorPool{};
		std::vector<VkDescriptorSet> m_DescriptorSets{};

		void CreateVertexBuffer(const GameContext& context)
		{
			const VkDeviceSize bufferSize = sizeof(V) * m_Vertices.size();

			CreateBuffer(context, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				m_StagingBuffer, m_StagingBufferMemory);

			UpdateVertexBuffer(context, bufferSize, m_StagingBufferMemory);

			CreateBuffer(context, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_VertexBuffer,
				m_VertexBufferMemory);

			CopyBuffer(context, m_StagingBuffer, m_VertexBuffer, bufferSize);
		}
		void CreateUniformBuffers(const GameContext& context)
		{
			VkDeviceSize bufferSize = sizeof(UniformBufferObject);

			m_UniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
			m_UniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
			m_UniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

			for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
			{
				CreateBuffer(context, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_UniformBuffers[i],
					m_UniformBuffersMemory[i]);

				vkMapMemory(context.vulkanContext.device, m_UniformBuffersMemory[i], 0, bufferSize, 0, &m_UniformBuffersMapped[i]);
			}
		}

		void CreateDescriptorPool(const GameContext& context)
		{
			std::vector< VkDescriptorPoolSize> poolSizes{};
			if (m_Info.usesUbo)
				poolSizes.push_back({ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT) });
			if (m_Info.texture)
				poolSizes.push_back({ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT) });

			if (poolSizes.empty())
				return;

			VkDescriptorPoolCreateInfo poolInfo{};
			poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
			poolInfo.pPoolSizes = poolSizes.data();
			poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

			if (vkCreateDescriptorPool(context.vulkanContext.device, &poolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS) {
				throw std::runtime_error("failed to create descriptor pool!");
			}
		}
		void CreateDescriptorSets(const GameContext& context, VkDescriptorSetLayout layout)
		{
			const std::vector layouts(MAX_FRAMES_IN_FLIGHT, layout);
			VkDescriptorSetAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocInfo.descriptorPool = m_DescriptorPool;
			allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
			allocInfo.pSetLayouts = layouts.data();

			m_DescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
			if (vkAllocateDescriptorSets(context.vulkanContext.device, &allocInfo, m_DescriptorSets.data()) != VK_SUCCESS) {
				throw std::runtime_error("failed to allocate descriptor sets!");
			}

			for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
			{
				VkDescriptorBufferInfo bufferInfo{};
				if (m_Info.usesUbo)
				{
					bufferInfo.buffer = m_UniformBuffers[i];
					bufferInfo.offset = 0;
					bufferInfo.range = sizeof(UniformBufferObject);
				}

				VkDescriptorImageInfo imageInfo{};
				if (m_Info.texture)
				{
					//const auto texture = ContentManager::GetInstance().LoadTexture(context, "Resources/Textures/grass_side.png");
					const auto texture = ContentManager::GetInstance().LoadTexture(context, "Resources/Textures/viking_room.png");

					imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
					imageInfo.imageView = m_Info.texture->GetTextureImageView();
					imageInfo.sampler = m_Info.texture->GetTextureSampler();
				}

				std::vector<VkWriteDescriptorSet> descriptorWrites{};
				if (m_Info.usesUbo)
				{
					VkWriteDescriptorSet descriptorWrite{};
					descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					descriptorWrite.dstSet = m_DescriptorSets[i];
					descriptorWrite.dstBinding = 0;
					descriptorWrite.dstArrayElement = 0;
					descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
					descriptorWrite.descriptorCount = 1;
					descriptorWrite.pBufferInfo = &bufferInfo;
					descriptorWrite.pImageInfo = &imageInfo;

					descriptorWrites.push_back(descriptorWrite);
				}
				if (m_Info.texture)
				{
					VkWriteDescriptorSet descriptorWrite{};
					descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					descriptorWrite.dstSet = m_DescriptorSets[i];
					descriptorWrite.dstBinding = 1;
					descriptorWrite.dstArrayElement = 0;
					descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
					descriptorWrite.descriptorCount = 1;
					descriptorWrite.pImageInfo = &imageInfo;

					descriptorWrites.push_back(descriptorWrite);
				}

				if (descriptorWrites.empty())
					return;

				vkUpdateDescriptorSets(context.vulkanContext.device, static_cast<uint32_t>(descriptorWrites.size()),
									   descriptorWrites.data(), 0, nullptr);
			}
		}

		void UpdateVertexBuffer(const GameContext& context, VkDeviceSize size, VkDeviceMemory bufferMemory)
		{
			void* data;
			vkMapMemory(context.vulkanContext.device, bufferMemory, 0, size, 0, &data);
			memcpy(data, m_Vertices.data(), sizeof(V) * m_Vertices.size());
			vkUnmapMemory(context.vulkanContext.device, bufferMemory);
		}

		static void CopyBuffer(const GameContext& context, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
		{
			const auto commandBuffer = CommandBuffer::StartSingleTimeCommands(context);

			VkBufferCopy copyRegion;
			copyRegion.srcOffset = 0; // Optional
			copyRegion.dstOffset = 0; // Optional
			copyRegion.size = size;
			vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

			CommandBuffer::StopSingleTimeCommands(context, commandBuffer);
		}

		void PrintMat4(const glm::mat4& matrix) const
		{
			const float* matPtr = glm::value_ptr(matrix);

			std::cout << "Matrix 4x4:" << std::endl;
			for (int i = 0; i < 4; ++i) 
			{
				for (int j = 0; j < 4; ++j) 
				{
					std::cout << std::fixed << std::setprecision(3) << matPtr[i * 4 + j] << "\t";
				}
				std::cout << '\n';
			}
		}
	};
}

#endif // MESH_H