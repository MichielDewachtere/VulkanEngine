#ifndef MESHINDEXED_H
#define MESHINDEXED_H

#include <vulkan/vulkan_core.h>

#include "Mesh.h"

template <typename T>
class MeshIndexed final : public Mesh<T>
{
public:
	explicit MeshIndexed(uint32_t vertexCapacity, uint32_t indexCapacity, bool is2d)
		: Mesh<T>(vertexCapacity, is2d)
		, m_IndexCapacity(indexCapacity)
	{
	}

	virtual ~MeshIndexed() override = default;

	MeshIndexed(const MeshIndexed&) = delete;
	MeshIndexed& operator=(const MeshIndexed&) = delete;
	MeshIndexed(MeshIndexed&&) = delete;
	MeshIndexed& operator=(MeshIndexed&&) = delete;

	virtual void Init(const GameContext& context) override
	{
		CreateIndexBuffer(context);
		Mesh<T>::Init(context);
	}
	virtual void CleanUp(const GameContext& context) override
	{
		vkDestroyBuffer(context.vulkanContext.device, m_IndexBuffer, nullptr);
		vkFreeMemory(context.vulkanContext.device, m_IndexBufferMemory, nullptr);

		Mesh<T>::CleanUp(context);
	}

	virtual void Draw(VkCommandBuffer commandBuffer) override
	{
		VkBuffer vertexBuffers[] = { Mesh<T>::m_VertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

		vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer, 0, VK_INDEX_TYPE_UINT16);

		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(m_Indices.size()), 1, 0, 0, 0);
	}

	void AddIndex(uint16_t index);

private:
	uint32_t m_IndexCapacity;
	VkBuffer m_IndexBuffer{ nullptr };
	VkDeviceMemory m_IndexBufferMemory{ nullptr };
	std::vector<uint16_t> m_Indices;

	void CreateIndexBuffer(const GameContext& context)
	{
		VkDeviceSize bufferSize = sizeof(uint16_t) * m_Indices.size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		Mesh<T>::CreateBuffer(context, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer,
			stagingBufferMemory);

		UpdateIndexBuffer(context, bufferSize, stagingBufferMemory);

		Mesh<T>::CreateBuffer(context, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_IndexBuffer, m_IndexBufferMemory);

		Mesh<T>::CopyBuffer(context, stagingBuffer, m_IndexBuffer, bufferSize);

		vkDestroyBuffer(context.vulkanContext.device, stagingBuffer, nullptr);
		vkFreeMemory(context.vulkanContext.device, stagingBufferMemory, nullptr);
	}
	void UpdateIndexBuffer(const GameContext& context, VkDeviceSize size, VkDeviceMemory bufferMemory)
	{
		void* data;
		vkMapMemory(context.vulkanContext.device, bufferMemory, 0, size, 0, &data);
		memcpy(data, m_Indices.data(), size);
		vkUnmapMemory(context.vulkanContext.device, bufferMemory);

	}
};
template <typename T>
void MeshIndexed<T>::AddIndex(uint16_t index)
{
	m_Indices.push_back(index);
}

#endif // MESHINDEXED_H