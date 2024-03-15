#ifndef MESH_H
#define MESH_H

#include <vulkan/vulkan_core.h>

#include "Util/Structs.h"

class Material;

class Mesh final
{
public:
	explicit Mesh(uint32_t vertexCapacity = 3);
	~Mesh() = default;

	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;
	Mesh(Mesh&&) = delete;
	Mesh& operator=(Mesh&&) = delete;

	void Init(const GameContext& context);
	void CleanUp(const GameContext& context);

	void Draw(VkCommandBuffer commandBuffer);

	void AddVertex(PosCol2D vertex);

	void AddTriangle(TrianglePosCol triangle, bool updateBuffer = false);
	void AddTriangle(PosCol2D vertex1, PosCol2D vertex2, PosCol2D vertex3, bool updateBuffer = false);

	void AddQuad(QuadPosCol quad, bool updateBuffer = false);
	void AddQuad(PosCol2D vertex1, PosCol2D vertex2, PosCol2D vertex3, PosCol2D vertex4, bool updateBuffer = false);

	static uint32_t FindMemoryType(const GameContext& context, uint32_t typeFilter, VkMemoryPropertyFlags properties);

private:
	uint8_t m_Id{ 0 };
	uint32_t m_VertexCapacity;
	//std::vector<TrianglePosCol> m_VecTriangles{};
	std::vector<PosCol2D> m_Vertices{};
	VkBuffer m_VertexBuffer{ nullptr }, m_StagingBuffer{ nullptr };
	VkDeviceMemory m_VertexBufferMemory{ nullptr }, m_StagingBufferMemory{ nullptr };

	void CreateBuffer(const GameContext& context, VkDeviceSize size, VkBufferUsageFlags usage,
	                  VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void CreateVertexBuffer(const GameContext& context);

	void UpdateBuffer(const GameContext& context, VkDeviceSize size, VkDeviceMemory bufferMemory);
	void CopyBuffer(const GameContext& context, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
};

#endif // MESH_H