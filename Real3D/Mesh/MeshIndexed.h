#ifndef MESHINDEXED_H
#define MESHINDEXED_H

#include <vulkan/vulkan_core.h>

#include "Util/Concepts.h"

#include "Mesh.h"

namespace real
{
	template <vertex_type V, typename Ubo>
	class MeshIndexed final : public Mesh<V, Ubo>
	{
	public:
		explicit MeshIndexed(GameObject* pOwner, MeshInfo info)
			: Mesh<V, Ubo>(pOwner, info)
		{
			m_IndexBuffers.push_back({});
		}
		virtual ~MeshIndexed() override
		{
			const auto context = RealEngine::GetGameContext();

			for (auto& [isDirty, buffer, allocation, data] : m_IndexBuffers)
			{
				if (buffer == nullptr)
					continue;

				vmaDestroyBuffer(context.vulkanContext.allocator, buffer, allocation);
				buffer = nullptr;
			}

		}

		MeshIndexed(const MeshIndexed&) = delete;
		MeshIndexed& operator=(const MeshIndexed&) = delete;
		MeshIndexed(MeshIndexed&&) = delete;
		MeshIndexed& operator=(MeshIndexed&&) = delete;

		virtual void Init(const GameContext& context) override
		{
			CreateIndexBuffer(context, 0);
			Mesh<V, Ubo>::Init(context);
		}

		virtual void Update() override
		{
			Mesh<V, Ubo>::Update();

			if (m_IndexBufferIsDirty == false)
				return;

			for (auto& [isDirty, buffer, memory, data] : m_IndexBuffers)
			{
				if (isDirty && data.empty() == false)
				{
					uint32_t offset = data.front();
					std::transform(data.begin(), data.end(), data.begin(), [offset](uint32_t& i) { return i - offset; });

					UpdateIndexBuffer(data, buffer);
					isDirty = false;
				}
			}

			m_IndexBufferIsDirty = false;
		}
		virtual void Render() override
		{
			const auto commandBuffer = CommandPool::GetInstance().GetActiveCommandBuffer();

			Mesh<V, Ubo>::m_pMaterial->Bind(commandBuffer, Mesh<V, Ubo>::m_Reference);
			Mesh<V, Ubo>::m_pMaterial->UpdateShaderVariables(this, Mesh<V, Ubo>::m_Reference);

			for (size_t i = 0; i < m_IndexBuffers.size(); ++i)
			{
				// Bind the vertex buffer
				const VkBuffer vertexBuffers[] = { Mesh<V, Ubo>::m_VertexBuffers[i].buffer };
				VkDeviceSize offsets[] = { 0 };
				vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

				// Bind the index buffer
				vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffers[i].buffer, 0, VK_INDEX_TYPE_UINT32);
				vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(m_IndexBuffers[i].data.size()), 1, 0, 0, 0);
			}
		}

		virtual void Kill() override
		{
			const auto context = RealEngine::GetGameContext();

			for (auto& [isDirty, buffer, allocation, data] : m_IndexBuffers)
			{
				vmaDestroyBuffer(context.vulkanContext.allocator, buffer, allocation);
				buffer = nullptr;
			}

			Mesh<V, Ubo>::Kill();
		}

		void AddIndex(uint32_t index)
		{
			if (m_IndexBuffers.back().data.size() == Mesh<V, Ubo>::m_Info.indexCapacity)
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
			auto v = idcs;

			auto addIndexBuffers = [&]() {
				while (!v.empty())
				{
					m_IndexBuffers.push_back({});
					FillUntilSize(v, m_IndexBuffers.back().data, Mesh<V, Ubo>::m_Info.indexCapacity);
					CreateIndexBuffer(RealEngine::GetGameContext(), m_IndexBuffers.size() - 1);
					m_IndexBuffers.back().isDirty = true;
				}
				};

			if (m_IndexBuffers.back().data.size() == Mesh<V, Ubo>::m_Info.indexCapacity)
			{
				addIndexBuffers();
			}
			else
			{
				FillUntilSize(v, m_IndexBuffers.back().data, Mesh<V, Ubo>::m_Info.indexCapacity);
				m_IndexBuffers.back().isDirty = true;

				addIndexBuffers();
			}

			m_IndexBufferIsDirty = true;
		}
		void SetIndices(const std::vector<uint32_t>& idcs)
		{
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
				FillUntilSize(v, m_IndexBuffers[counter].data, Mesh<V, Ubo>::m_Info.indexCapacity);
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
			m_IndexBuffers.erase(m_IndexBuffers.begin() + 1, m_IndexBuffers.end());
			m_IndexBuffers.front().data.clear();

			m_IndexBufferIsDirty = true;
		}

	private:
		bool m_IndexBufferIsDirty{ false };

		std::vector<BufferContext<uint32_t>> m_IndexBuffers{};

		void CreateIndexBuffer(const GameContext& context, size_t index)
		{
			VkDeviceSize bufferSize = sizeof(uint32_t) * Mesh<V, Ubo>::m_Info.indexCapacity;

			VkBuffer stagingBuffer;
			VmaAllocation stagingBufferAllocation;
			CreateBuffer(context, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer,
				stagingBufferAllocation);

			Mesh<V, Ubo>::UpdateBuffer<uint32_t>(stagingBufferAllocation, m_IndexBuffers[index].data);

			VkBuffer indexBuffer;
			VmaAllocation bufferAllocation;
			CreateBuffer(context, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, bufferAllocation);

			Mesh<V, Ubo>::CopyBuffer(context, stagingBuffer, indexBuffer, bufferSize);

			m_IndexBuffers[index].buffer = indexBuffer;
			m_IndexBuffers[index].allocation = bufferAllocation;

			vmaDestroyBuffer(context.vulkanContext.allocator, stagingBuffer, stagingBufferAllocation);
		}

		void UpdateIndexBuffer(const std::vector<uint32_t>& data, VkBuffer buffer)
		{
			const VkDeviceSize bufferSize = sizeof(uint32_t) * Mesh<V, Ubo>::m_Info.indexCapacity;
			const auto context = RealEngine::GetGameContext();

			VkBuffer stagingBuffer;
			VmaAllocation stagingBufferAllocation;
			CreateBuffer(context, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				stagingBuffer, stagingBufferAllocation);

			Mesh<V, Ubo>::UpdateBuffer<uint32_t>(stagingBufferAllocation, data);

			Mesh<V, Ubo>::CopyBuffer(context, stagingBuffer, buffer, bufferSize);

			vmaDestroyBuffer(context.vulkanContext.allocator, stagingBuffer, stagingBufferAllocation);
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
	};
}

#endif // MESHINDEXED_H