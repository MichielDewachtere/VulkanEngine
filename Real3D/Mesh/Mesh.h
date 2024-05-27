#ifndef MESH_H
#define MESH_H

#include <iomanip>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <vulkan/vulkan_core.h>

#include <real_core/GameObject.h>

#include "Core/CommandPool.h"
#include "Content/ContentManager.h"
#include "Util/Concepts.h"
#include "Util/Structs.h"
#include "Util/VulkanUtil.h"
#include "RealEngine.h"

#include "Util/vk_mem_alloc.h"

#include "BaseMesh.h"

namespace real
{
	template <vertex_type V, typename Ubo>
	class Mesh : public BaseMesh
	{
	public:
		explicit Mesh(real::GameObject* pOwner, MeshInfo info)
			: BaseMesh(pOwner, info)
		{
			m_VertexBuffers.push_back({});
		}

		virtual ~Mesh() override
		{
			const auto context = RealEngine::GetGameContext();

			for (auto& vertexBuffer : m_VertexBuffers)
			{
				if (vertexBuffer.buffer != nullptr)
				{
					vmaDestroyBuffer(context.vulkanContext.allocator, vertexBuffer.buffer, vertexBuffer.allocation);
					vertexBuffer.buffer = nullptr;
				}
			}
		}

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
				if (isDirty && data.empty() == false)
				{
					UpdateVertexBuffer(data, buffer);
					isDirty = false;
				}
			}

			m_VertexBufferIsDirty = false;
		}
		virtual void Render() override
		{
			const auto commandBuffer = CommandPool::GetInstance().GetActiveCommandBuffer();

			m_pMaterial->Bind(commandBuffer, m_Reference);
			m_pMaterial->UpdateShaderVariables(this, m_Reference);

			const auto buffers = GetAllVertexBuffers();
			const std::vector<VkDeviceSize> offsets(buffers.size(), 0);
			vkCmdBindVertexBuffers(commandBuffer, 0, static_cast<uint32_t>(buffers.size()), buffers.data(), offsets.data());

			const auto vertices = GetAllVertices();
			vkCmdDraw(commandBuffer, static_cast<uint32_t>(vertices.size()), 1, 0, 0);
		}

		virtual void Kill() override
		{
			const auto context = RealEngine::GetGameContext();

			m_pMaterial->CleanUpUbo(m_Reference);

			for (auto& vertexBuffer : m_VertexBuffers)
			{
				vmaDestroyBuffer(context.vulkanContext.allocator, vertexBuffer.buffer, vertexBuffer.allocation);
				vertexBuffer.buffer = nullptr;
			}
		}

		void SetMaterial(Material<Ubo>* material);

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
					FillUntilSize(v, m_VertexBuffers.back().data, m_Info.vertexCapacity);
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
				FillUntilSize(v, m_VertexBuffers.back().data, m_Info.vertexCapacity);
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

				FillUntilSize(v, m_VertexBuffers[counter].data, m_Info.vertexCapacity);
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
		Material<Ubo>* m_pMaterial{ nullptr };
		uint32_t m_Reference{ 0 };

		bool m_VertexBufferIsDirty{ false };
		std::vector<BufferContext<V>> m_VertexBuffers;

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

		static void CopyBuffer(const GameContext& context, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

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

	template <vertex_type V, typename Ubo>
	void Mesh<V, Ubo>::SetMaterial(Material<Ubo>* material)
	{
		m_pMaterial = material;
		m_Reference = m_pMaterial->AddReference();
	}

	template <vertex_type V, typename Ubo>
	void Mesh<V, Ubo>::CopyBuffer(const GameContext& context, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
	{
		const auto commandBuffer = CommandBuffer::StartSingleTimeCommands(context);

		VkBufferCopy copyRegion{};
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

		CommandBuffer::StopSingleTimeCommands(context, commandBuffer);
	}
}

#endif // MESH_H