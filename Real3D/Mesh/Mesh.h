#ifndef MESH_H
#define MESH_H

#include <iomanip>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <vulkan/vulkan_core.h>

#include <real_core/Component.h>
#include <real_core/DrawableComponent.h>
#include <real_core/GameObject.h>

#include "Misc/CameraManager.h"
#include "Misc/Camera.h"
#include "Core/CommandPool.h"
#include "Content/ContentManager.h"
#include "Util/Concepts.h"
#include "Util/Structs.h"
#include "Util/VulkanUtil.h"
#include "RealEngine.h"

#include "Util/vk_mem_alloc.h"

namespace real
{
	struct MeshInfo
	{
		uint32_t vertexCapacity = 0, indexCapacity = 0;
		Texture2D* texture = nullptr;
		bool usesUbo = false;
	};

	template <typename T>
	struct BufferContext
	{
		bool isDirty = false;
		VkBuffer buffer{ nullptr };
		VmaAllocation allocation{ nullptr };
		std::vector<T> data{};
	};

	template <vertex_type V>
	class Mesh : public DrawableComponent
	{
	public:
		explicit Mesh(real::GameObject* pOwner, MeshInfo info)
			: DrawableComponent(pOwner)
			, m_Info(info)
		{
			m_VertexBuffers.push_back({});
		}

		virtual ~Mesh() override = default;

		Mesh(const Mesh&) = delete;
		Mesh& operator=(const Mesh&) = delete;
		Mesh(Mesh&&) = delete;
		Mesh& operator=(Mesh&&) = delete;

		virtual void Init(const GameContext& context)
		{
			CreateVertexBuffer(context, 0);
		}

		virtual void Update() override
		{
			if (m_VertexBufferIsDirty == false)
				return;

			for (auto& [isDirty, buffer, memory, data] : m_VertexBuffers)
			{
				if (isDirty)
				{
					Mesh<V>::UpdateVertexBuffer(data, buffer);
					isDirty = false;
				}
			}

			m_VertexBufferIsDirty = false;
		}
		virtual void Render() override
		{
			const auto commandBuffer = CommandPool::GetInstance().GetActiveCommandBuffer();



			const auto buffers = GetAllVertexBuffers();
			const std::vector<VkDeviceSize> offsets(buffers.size(), 0);
			vkCmdBindVertexBuffers(commandBuffer, 0, static_cast<uint32_t>(buffers.size()), buffers.data(), offsets.data());

			const auto vertices = GetAllVertices();
			vkCmdDraw(commandBuffer, static_cast<uint32_t>(vertices.size()), 1, 0, 0);
		}

		virtual void Kill() override
		{
			const auto context = RealEngine::GetGameContext();

			for (size_t i = 0; i < m_UniformBuffers.size(); i++)
			{
				vmaUnmapMemory(context.vulkanContext.allocator, m_UniformBufferAllocations[i]);
				vmaDestroyBuffer(context.vulkanContext.allocator, m_UniformBuffers[i], m_UniformBufferAllocations[i]);
			}

			for (const auto& vertexBuffer : m_VertexBuffers)
			{
				vmaDestroyBuffer(context.vulkanContext.allocator, vertexBuffer.buffer, vertexBuffer.allocation);
			}

			if (m_Info.usesUbo)
			{
				vkDestroyDescriptorPool(context.vulkanContext.device, m_DescriptorPool, nullptr);
			}
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

		virtual void Draw(VkCommandBuffer /*commandBuffer*/)
		{
			const auto commandBuffer = CommandPool::GetInstance().GetActiveCommandBuffer();

			const auto buffers = GetAllVertexBuffers();
			const std::vector<VkDeviceSize> offsets(buffers.size(), 0);
			vkCmdBindVertexBuffers(commandBuffer, 0, static_cast<uint32_t>(buffers.size()), buffers.data(), offsets.data());

			const auto vertices = GetAllVertices();
			vkCmdDraw(commandBuffer, static_cast<uint32_t>(vertices.size()), 1, 0, 0);
		}

		void AddVertex(V vertex)
		{
			if (m_VertexBuffers.back().data.size() == m_Info.vertexCapacity)
			{
				m_VertexBuffers.back().data.push_back(vertex);
				m_VertexBuffers.back().isDirty = true;
			}
			else
			{
				m_VertexBuffers.push_back({});
				m_VertexBuffers.back().data.push_back(vertex);
				CreateVertexBuffer(RealEngine::GetGameContext(), m_VertexBuffers.size() - 1);
			}

			m_VertexBufferIsDirty = true;
		}
		void AddVertices(const std::vector<V>& vertices)
		{
			auto v = vertices;

			auto addVertexBuffers = [&]() {
				while (!v.empty())
				{
					m_VertexBuffers.push_back({});
					fillUntilSize(v, m_VertexBuffers.back().data, m_Info.vertexCapacity);
					CreateVertexBuffer(RealEngine::GetGameContext(), m_VertexBuffers.size() - 1);
					m_VertexBuffers.back().isDirty = true;
				}
				};

			if (m_VertexBuffers.back().data.size() == m_Info.vertexCapacity)
			{
				addVertexBuffers();
			}
			else
			{
				fillUntilSize(v, m_VertexBuffers.back().data, m_Info.vertexCapacity);
				m_VertexBuffers.back().isDirty = true;

				addVertexBuffers();
			}

			m_VertexBufferIsDirty = true;
		}
		void SetVertices(const std::vector<V>& vertices)
		{
			auto v = vertices;
			int counter = 0;

			while (v.empty() == false)
			{
				bool createNewBuffer = false;
				if (counter >= m_VertexBuffers.size())
				{
					m_VertexBuffers.push_back({});
					createNewBuffer = true;
				}

				m_VertexBuffers[counter].data.clear();

				fillUntilSize(v, m_VertexBuffers[counter].data, m_Info.vertexCapacity);
				if (createNewBuffer)
					CreateVertexBuffer(RealEngine::GetGameContext(), counter);
				m_VertexBuffers[counter].isDirty = true;

				++counter;
			}

			if (counter < m_VertexBuffers.size() - 1)
			{
				m_VertexBuffers.erase(m_VertexBuffers.begin() + counter, m_VertexBuffers.end());
			}

			m_VertexBufferIsDirty = true;
		}
		void ClearVertices()
		{
			m_VertexBuffers.erase(m_VertexBuffers.begin() + 1, m_VertexBuffers.end());
			m_VertexBuffers.front().data.clear();
			
			m_VertexBufferIsDirty = true;
		}

	protected:
		MeshInfo m_Info;

		bool m_VertexBufferIsDirty{ false };
		std::vector<BufferContext<V>> m_VertexBuffers;

		// Uniform (Constant) buffer
		std::vector<VkBuffer> m_UniformBuffers{ nullptr };
		std::vector<VmaAllocation> m_UniformBufferAllocations{ nullptr };
		std::vector<void*> m_UniformBuffersMapped{ nullptr };

		// TODO: Ask => one per pipeline / one per mesh??
		// They use same UBO/Samplers but different data
		VkDescriptorPool m_DescriptorPool{};
		std::vector<VkDescriptorSet> m_DescriptorSets{};

		void CreateVertexBuffer(const GameContext& context, size_t index)
		{
			const VkDeviceSize bufferSize = sizeof(V) * m_Info.vertexCapacity;

			VkBuffer stagingBuffer;
			VmaAllocation stagingBufferAllocation;
			CreateBuffer(context, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				stagingBuffer, stagingBufferAllocation);

			UpdateBuffer<V>(stagingBufferAllocation, m_VertexBuffers[index].data);

			VkBuffer vertexBuffer;
			VmaAllocation bufferAllocation;
			CreateBuffer(context, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer,
				bufferAllocation);

			CopyBuffer(context, stagingBuffer, vertexBuffer, bufferSize);

			m_VertexBuffers[index].buffer = vertexBuffer;
			m_VertexBuffers[index].allocation = bufferAllocation;

			vmaDestroyBuffer(context.vulkanContext.allocator, stagingBuffer, stagingBufferAllocation);
		}
		void CreateUniformBuffers(const GameContext& context)
		{
			VkDeviceSize bufferSize = sizeof(UniformBufferObject);

			m_UniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
			m_UniformBufferAllocations.resize(MAX_FRAMES_IN_FLIGHT);
			m_UniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

			for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
			{
				CreateBuffer(context, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_UniformBuffers[i],
					m_UniformBufferAllocations[i]);

				vmaMapMemory(context.vulkanContext.allocator, m_UniformBufferAllocations[i], &m_UniformBuffersMapped[i]);
			}
		}

		void CreateDescriptorPool(const GameContext& context)
		{
			std::vector<VkDescriptorPoolSize> poolSizes{};
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


		template <typename T>
		static void UpdateBuffer(VmaAllocation allocation, const std::vector<T>& v)
		{
			void* data;
			const auto context = RealEngine::GetGameContext();
			vmaMapMemory(context.vulkanContext.allocator, allocation, &data);
			memcpy(data, v.data(), sizeof(T) * v.size());
			vmaUnmapMemory(context.vulkanContext.allocator, allocation);
		}

		void UpdateVertexBuffer(const std::vector<V>& data, VkBuffer buffer)
		{
			const VkDeviceSize bufferSize = sizeof(V) * m_Info.vertexCapacity;
			const auto context = RealEngine::GetGameContext();

			VkBuffer stagingBuffer;
			VmaAllocation stagingBufferAllocation;
			CreateBuffer(context, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				stagingBuffer, stagingBufferAllocation);

			UpdateBuffer<V>(stagingBufferAllocation, data);

			CopyBuffer(context, stagingBuffer, buffer, bufferSize);

			vmaDestroyBuffer(context.vulkanContext.allocator, stagingBuffer, stagingBufferAllocation);
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

		std::vector<VkBuffer> GetAllVertexBuffers()
		{
			std::vector<VkBuffer> v(m_VertexBuffers.size());

			std::ranges::transform(m_VertexBuffers, v.begin(),
				[](const BufferContext<V>& vertexBuffer)
				{
					return vertexBuffer.buffer;
				});

			return v;
		}
		std::vector<V> GetAllVertices()
		{
			std::vector<V> v;

			for (const auto & vertexBuffer : m_VertexBuffers)
			{
				v.insert(v.end(), vertexBuffer.data.begin(), vertexBuffer.data.end());
			}

			return v;
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