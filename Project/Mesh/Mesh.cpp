#include "Mesh.h"

#include <stdexcept>

#include "Graphics/Renderer.h"
#include "Core/CommandPool.h"
#include "Material/Material.h"

Mesh::Mesh(uint32_t vertexCapacity)
	: m_VertexCapacity(vertexCapacity)
{
}

void Mesh::Init(const GameContext& context)
{
	CreateVertexBuffer(context);
}

void Mesh::CleanUp(const GameContext& context)
{
	vkDestroyBuffer(context.vulkanContext.device, m_VertexBuffer, nullptr);
	vkFreeMemory(context.vulkanContext.device, m_VertexBufferMemory, nullptr);

	vkDestroyBuffer(context.vulkanContext.device, m_StagingBuffer, nullptr);
	vkFreeMemory(context.vulkanContext.device, m_StagingBufferMemory, nullptr);
}

void Mesh::Draw(VkCommandBuffer commandBuffer)
{
	VkBuffer vertexBuffers[] = { m_VertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

	vkCmdDraw(commandBuffer, static_cast<uint32_t>(m_Vertices.size()), 1, 0, 0);
}

void Mesh::CreateBuffer(const GameContext& context, VkDeviceSize size, VkBufferUsageFlags usage,
	VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
	// Create vertex buffer
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(context.vulkanContext.device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create vertex buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(context.vulkanContext.device, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = FindMemoryType(context, memRequirements.memoryTypeBits, properties);

	// TODO: This could be improved -> see Conclusion @ https://vulkan-tutorial.com/Vertex_buffers/Staging_buffer
	if (vkAllocateMemory(context.vulkanContext.device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate vertex buffer memory!");
	}

	vkBindBufferMemory(context.vulkanContext.device, buffer, bufferMemory, 0);
}

void Mesh::CreateVertexBuffer(const GameContext& context)
{
	const VkDeviceSize bufferSize = sizeof(PosCol2D) * m_Vertices.size();
#pragma region Staging Buffer

	CreateBuffer(context, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
	             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
	             m_StagingBuffer, m_StagingBufferMemory);

	UpdateBuffer(context, bufferSize, m_StagingBufferMemory);

	CreateBuffer(context, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
	             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_VertexBuffer,
	             m_VertexBufferMemory);

	CopyBuffer(context, m_StagingBuffer, m_VertexBuffer, bufferSize);
#pragma endregion

//#pragma region Vertex Buffer
//	CreateBuffer(context, bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
//	             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
//			m_VertexBuffer, m_VertexBufferMemory);
//
//	void* data;
//	UpdateBuffer(context, bufferSize, m_VertexBufferMemory);
//#pragma endregion
}

void Mesh::UpdateBuffer(const GameContext& context, VkDeviceSize size, VkDeviceMemory bufferMemory)
{
	void* data;
	vkMapMemory(context.vulkanContext.device, bufferMemory, 0, size, 0, &data);
	memcpy(data, m_Vertices.data(), sizeof(PosCol2D) * m_Vertices.size());
	vkUnmapMemory(context.vulkanContext.device, bufferMemory);
}

void Mesh::CopyBuffer(const GameContext& context, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	// TODO: Could make seperate command pool for short lived buffers
	allocInfo.commandPool = CommandPool::GetInstance().GetCommandPool();
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(context.vulkanContext.device, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = 0; // Optional
	copyRegion.dstOffset = 0; // Optional
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	const VkQueue graphicsQueue = Renderer::GetInstance().GetGraphicsQueue();
	vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(graphicsQueue);

	vkFreeCommandBuffers(context.vulkanContext.device, CommandPool::GetInstance().GetCommandPool(), 1, &commandBuffer);
}

void Mesh::AddVertex(PosCol2D vertex)
{
	m_Vertices.push_back(vertex);
}

void Mesh::AddTriangle(TrianglePosCol triangle, bool updateBuffer)
{
	AddVertex(triangle.vertex1);
	AddVertex(triangle.vertex2);
	AddVertex(triangle.vertex3);

	//m_VecTriangles.push_back(triangle);
	//if (updateBuffer && m_IsInitialized)
	//	UpdateBuffer();
}

void Mesh::AddTriangle(PosCol2D vertex1, PosCol2D vertex2, PosCol2D vertex3, bool updateBuffer)
{
	AddTriangle(TrianglePosCol(vertex1, vertex2, vertex3), updateBuffer);
}

void Mesh::AddQuad(QuadPosCol quad, bool updateBuffer)
{
	AddTriangle(TrianglePosCol(quad.vertex1, quad.vertex2, quad.vertex3), false);
	AddTriangle(TrianglePosCol(quad.vertex3, quad.vertex4, quad.vertex1), updateBuffer);
}

void Mesh::AddQuad(PosCol2D vertex1, PosCol2D vertex2, PosCol2D vertex3, PosCol2D vertex4, bool updateBuffer)
{
	AddTriangle(TrianglePosCol(vertex1, vertex2, vertex3), false);
	AddTriangle(TrianglePosCol(vertex3, vertex4, vertex1), updateBuffer);
}

uint32_t Mesh::FindMemoryType(const GameContext& context, uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(context.vulkanContext.physicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}
