#ifndef TRANSPARENTMODEL_H
#define TRANSPARENTMODEL_H

#include <array>
#include <numeric>
#include <vector>
#include <real_core/DrawableComponent.h>

#include "Util/VulkanUtil.h"

#include "Material/Material.h"
#include "Materials/TransparentMaterial.h"
#include "Materials/WaterMaterial.h"
#include "Materials/TranspriteMaterial.h"
#include "Mesh/BaseMesh.h"
#include "Util/Structs.h"

enum class TransparencyType
{
	water = 0,
	transparentTexture = 1,
	transparentSprite = 2,
	
	none = 3,
};

struct TransparentFace
{
	std::array<real::PosTexNorm, 4> vertices;
	std::array<uint32_t, 6> idcs = { 0,1,2,2,3,0 };
	TransparencyType type{};
	glm::ivec3 center{};

	TransparentFace(const std::array<real::PosTexNorm, 4> _faces, const TransparencyType _type)
		: vertices(_faces), type(_type)
	{
		const glm::vec3 sum = std::accumulate(vertices.begin(), vertices.end(), glm::vec3(0.0f, 0.0f, 0.0f),
			[](const glm::vec3& acc, const real::PosTexNorm& ptn)
			{
				return acc + ptn.pos;
			});
		center = sum / static_cast<float>(vertices.size());
	}
};

class TransparentModel final : public real::DrawableComponent
{
public:
	explicit TransparentModel(real::GameObject* pOwner, uint32_t vertexCapacity = 512, uint32_t indexCapacity = 512);
	virtual ~TransparentModel() override;

	TransparentModel(const TransparentModel&) = delete;
	TransparentModel& operator=(const TransparentModel&) = delete;
	TransparentModel(TransparentModel&&) = delete;
	TransparentModel& operator=(TransparentModel&&) = delete;

	virtual void Update() override;
	virtual void Render() override;

	virtual void Kill() override;

	void AddFaces(const std::vector<TransparentFace>& faces);
	void SetFaces(const std::vector<TransparentFace>& faces);
	void ClearFaces();

	void SortFaces(const glm::ivec3& position, bool sortBlocks = true);

private:
	uint32_t m_VertexCapacity, m_IndexCapacity;

	real::BaseMaterial* m_pCurrentMaterial{ nullptr };
	TransparentMaterial* m_pTransparentMaterial{ nullptr };
	WaterMaterial* m_pWaterMaterial{ nullptr };
	TranspriteMaterial* m_pTranspriteMaterial{ nullptr };

	std::vector<real::PosTexNorm> m_Vertices{};
	std::vector<uint32_t> m_Indices{};
	std::vector<std::tuple<uint32_t, uint32_t, TransparencyType>> m_Regions;

	uint32_t m_WaterReference{ 0 }, m_TransparentReference{ 0 }, m_TranspriteReference{ 0 };

	bool m_BuffersAreDirty{ false };
	std::vector<real::BufferContext<real::PosTexNorm>> m_VertexBuffers;
	std::vector<real::BufferContext<uint32_t>> m_IndexBuffers;

	std::vector<TransparentFace> m_Faces;

	template <typename T>
	void CreateBuffer(std::vector<real::BufferContext<T>>& buffers, size_t index, uint32_t capacity, bool isVertexBuffer);
	template <typename T>
	void UpdateBuffer(const std::vector<T>& data, VkBuffer buffer, uint32_t capacity);
	template <typename T>
	static void UpdateBufferHelper(VmaAllocation allocation, const std::vector<T>& v);
	static void CopyBuffer(const real::GameContext& context, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
};

template <typename T>
void TransparentModel::CreateBuffer(std::vector<real::BufferContext<T>>& buffers, size_t index, uint32_t capacity, bool isVertexBuffer)
{
	const auto context = real::RealEngine::GetGameContext();

	const VkDeviceSize bufferSize = sizeof(T) * capacity;

	VkBuffer stagingBuffer;
	VmaAllocation stagingBufferAllocation;
	real::CreateBuffer(context, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer, stagingBufferAllocation);

	UpdateBufferHelper<T>(stagingBufferAllocation, buffers[index].data);

	VkBuffer buffer;
	VmaAllocation bufferAllocation;
	real::CreateBuffer(context, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | (isVertexBuffer ? VK_BUFFER_USAGE_VERTEX_BUFFER_BIT : VK_BUFFER_USAGE_INDEX_BUFFER_BIT),
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer,
		bufferAllocation);

	CopyBuffer(context, stagingBuffer, buffer, bufferSize);

	buffers[index].buffer = buffer;
	buffers[index].allocation = bufferAllocation;

	vmaDestroyBuffer(context.vulkanContext.allocator, stagingBuffer, stagingBufferAllocation);
}

template <typename T>
void TransparentModel::UpdateBuffer(const std::vector<T>& data, VkBuffer buffer, uint32_t capacity)
{
	const VkDeviceSize bufferSize = sizeof(T) * capacity;
	const auto context = real::RealEngine::GetGameContext();

	VkBuffer stagingBuffer;
	VmaAllocation stagingBufferAllocation;
	real::CreateBuffer(context, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer, stagingBufferAllocation);

	UpdateBufferHelper<T>(stagingBufferAllocation, data);

	CopyBuffer(context, stagingBuffer, buffer, bufferSize);

	vmaDestroyBuffer(context.vulkanContext.allocator, stagingBuffer, stagingBufferAllocation);
}

template <typename T>
void TransparentModel::UpdateBufferHelper(VmaAllocation allocation, const std::vector<T>& v)
{
	void* data;
	const auto context = real::RealEngine::GetGameContext();
	vmaMapMemory(context.vulkanContext.allocator, allocation, &data);
	memcpy(data, v.data(), sizeof(T) * v.size());
	vmaUnmapMemory(context.vulkanContext.allocator, allocation);
}

#endif // TRANSPARENTMODEL_H
