#ifndef MESHINDEXED_H
#define MESHINDEXED_H

#include <vulkan/vulkan_core.h>

#include "Util/Concepts.h"

#include "Mesh.h"

namespace real
{
	template <vertex_type V>
	class MeshIndexed final : public Mesh<V>
	{
	public:
		explicit MeshIndexed(real::GameObject* pOwner, MeshInfo info)
			: Mesh<V>(pOwner, info)
		{
			m_IndexBuffers.push_back({});
		}
		virtual ~MeshIndexed() override = default;

		MeshIndexed(const MeshIndexed&) = delete;
		MeshIndexed& operator=(const MeshIndexed&) = delete;
		MeshIndexed(MeshIndexed&&) = delete;
		MeshIndexed& operator=(MeshIndexed&&) = delete;

		virtual void Init(const GameContext& context) override
		{
			CreateIndexBuffer(context, 0);
			Mesh<V>::Init(context);
		}

		virtual void Update() override
		{
			Mesh<V>::Update();

			if (m_IndexBufferIsDirty == false)
				return;

			for (auto& [isDirty, buffer, memory, data] : m_IndexBuffers)
			{
				if (isDirty)
				{
					uint32_t offset = data.front();
					std::transform(data.begin(), data.end(), data.begin(), [offset](uint32_t& i) { return i - offset; });

					UpdateIndexBuffer(data, buffer);
					isDirty = false;
				}
			}

			m_IndexBufferIsDirty = false;
		}

		virtual void Kill() override
		{
			const auto context = RealEngine::GetGameContext();

			for (const auto& indexBuffer : m_IndexBuffers)
			{
				vmaDestroyBuffer(context.vulkanContext.allocator, indexBuffer.buffer, indexBuffer.allocation);
				//vkDestroyBuffer(context.vulkanContext.device, indexBuffer.buffer, nullptr);
				//vkFreeMemory(context.vulkanContext.device, indexBuffer.memory, nullptr);
			}

			Mesh<V>::Kill();
		}

		virtual void Draw(VkCommandBuffer commandBuffer) override
		{
			for (size_t i = 0; i < m_IndexBuffers.size(); ++i) 
			{
				// Bind the vertex buffer
				const VkBuffer vertexBuffers[] = { Mesh<V>::m_VertexBuffers[i].buffer };
				VkDeviceSize offsets[] = { 0 };
				vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

				// Bind the index buffer
				vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffers[i].buffer, 0, VK_INDEX_TYPE_UINT32);
				vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(m_IndexBuffers[i].data.size()), 1, 0, 0, 0);
			}
		}

		void AddIndex(uint32_t index)
		{
			//m_Indices.push_back(index);

			if (m_IndexBuffers.back().data.size() == Mesh<V>::m_Info.indexCapacity)
			{
				m_IndexBuffers.back().data.push_back(index);
				m_IndexBuffers.back().isDirty = true;
			}
			else
			{
				m_IndexBuffers.push_back({});
				m_IndexBuffers.back().data.push_back(index);
				CreateIndexBuffer(RealEngine::GetGameContext(), m_IndexBuffers.size() - 1);
			}

			m_IndexBufferIsDirty = true;
		}
		void AddIndices(const std::vector<uint32_t>& idcs)
		{
			//m_Indices.insert(m_Indices.end(), idcs.begin(), idcs.end());

			auto v = idcs;

			auto addIndexBuffers = [&]() {
				while (!v.empty())
				{
					m_IndexBuffers.push_back({});
					fillUntilSize(v, m_IndexBuffers.back().data, Mesh<V>::m_Info.indexCapacity);
					CreateIndexBuffer(RealEngine::GetGameContext(), m_IndexBuffers.size() - 1);
					m_IndexBuffers.back().isDirty = true;
				}
				};

			if (m_IndexBuffers.back().data.size() == Mesh<V>::m_Info.indexCapacity)
			{
				addIndexBuffers();
			}
			else
			{
				fillUntilSize(v, m_IndexBuffers.back().data, Mesh<V>::m_Info.indexCapacity);
				m_IndexBuffers.back().isDirty = true;

				addIndexBuffers();
			}

			m_IndexBufferIsDirty = true;
		}
		void SetIndices(const std::vector<uint32_t>& idcs)
		{
			//m_Indices = idcs;

			auto v = idcs;
			int counter = 0;

			while (v.empty() == false)
			{
				bool createNewBuffer = false;
				if (counter >= m_IndexBuffers.size())
				{
					m_IndexBuffers.push_back({});
					createNewBuffer = true;
				}

				m_IndexBuffers[counter].data.clear();
				fillUntilSize(v, m_IndexBuffers[counter].data, Mesh<V>::m_Info.indexCapacity);
				if (createNewBuffer)
					CreateIndexBuffer(RealEngine::GetGameContext(), counter);
				m_IndexBuffers[counter].isDirty = true;

				++counter;
			}

			if (counter < m_IndexBuffers.size() - 1)
			{
				m_IndexBuffers.erase(m_IndexBuffers.begin() + counter, m_IndexBuffers.end());
			}

			m_IndexBufferIsDirty = true;
		}
		void ClearIndices()
		{
			//m_Indices.clear();

			m_IndexBuffers.erase(m_IndexBuffers.begin() + 1, m_IndexBuffers.end());
			m_IndexBuffers.front().data.clear();

			m_IndexBufferIsDirty = true;
		}

	private:
		bool m_IndexBufferIsDirty{ false };

		std::vector<BufferContext<uint32_t>> m_IndexBuffers{};

		void CreateIndexBuffer(const GameContext& context, size_t index)
		{
			VkDeviceSize bufferSize = sizeof(uint32_t) * Mesh<V>::m_Info.indexCapacity;

			VkBuffer stagingBuffer;
			VmaAllocation stagingBufferAllocation;
			//VkDeviceMemory stagingBufferMemory;
			CreateBuffer(context, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer,
				stagingBufferAllocation);

			Mesh<V>::UpdateBuffer<uint32_t>(stagingBufferAllocation, m_IndexBuffers[index].data);

			VkBuffer indexBuffer;
			VmaAllocation bufferAllocation;
			//VkDeviceMemory indexBufferMemory;
			CreateBuffer(context, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, bufferAllocation);

			Mesh<V>::CopyBuffer(context, stagingBuffer, indexBuffer, bufferSize);

			m_IndexBuffers[index].buffer = indexBuffer;
			m_IndexBuffers[index].allocation = bufferAllocation;
			//m_IndexBuffers[index].memory = indexBufferMemory;

			vmaDestroyBuffer(context.vulkanContext.allocator, stagingBuffer, stagingBufferAllocation);
			//vkDestroyBuffer(context.vulkanContext.device, stagingBuffer, nullptr);
			//vkFreeMemory(context.vulkanContext.device, stagingBufferMemory, nullptr);
		}

		void UpdateIndexBuffer(const std::vector<uint32_t>& data, VkBuffer buffer)
		{
			const VkDeviceSize bufferSize = sizeof(uint32_t) * Mesh<V>::m_Info.indexCapacity;
			const auto context = RealEngine::GetGameContext();

			VkBuffer stagingBuffer;
			VmaAllocation stagingBufferAllocation;
			//VkDeviceMemory stagingBufferMemory;
			CreateBuffer(context, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				stagingBuffer, stagingBufferAllocation);

			Mesh<V>::UpdateBuffer<uint32_t>(stagingBufferAllocation, data);

			Mesh<V>::CopyBuffer(context, stagingBuffer, buffer, bufferSize);

			vmaDestroyBuffer(context.vulkanContext.allocator, stagingBuffer, stagingBufferAllocation);
			//vkDestroyBuffer(context.vulkanContext.device, stagingBuffer, nullptr);
			//vkFreeMemory(context.vulkanContext.device, stagingBufferMemory, nullptr);
		}

		std::vector<uint32_t> GetAllIndices()
		{
			std::vector<uint32_t> v;

			for (const auto& indexBuffer : m_IndexBuffers)
			{
				v.insert(v.end(), indexBuffer.data.begin(), indexBuffer.data.end());
			}

			return v;
		}


		//void UpdateIndexBuffer(const GameContext& context, VkDeviceSize size, VkDeviceMemory bufferMemory)
		//{
		//	void* data;
		//	vkMapMemory(context.vulkanContext.device, bufferMemory, 0, size, 0, &data);
		//	memcpy(data, m_Indices.data(), sizeof(uint32_t) * m_Indices.size());
		//	vkUnmapMemory(context.vulkanContext.device, bufferMemory);

		//}
	};
}

#endif // MESHINDEXED_H