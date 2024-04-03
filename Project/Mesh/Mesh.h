#ifndef MESH_H
#define MESH_H

#include <iomanip>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <vulkan/vulkan_core.h>

#include "Util/Structs.h"
#include "BaseMesh.h"
#include "Graphics/Renderer.h"
#include "Core/CommandPool.h"
#include "Material/Material.h"
#include "Misc/Camera.h"
#include "Util/GameTime.h"

class Material;

template <class T>
class Mesh : public BaseMesh
{
public:
	explicit Mesh(uint32_t vertexCapacity, bool is2d);
	virtual ~Mesh() override = default;

	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;
	Mesh(Mesh&&) = delete;
	Mesh& operator=(Mesh&&) = delete;

	virtual void Init(const GameContext& context) override;
	virtual void CleanUp(const GameContext& context) override;

	void Update(uint32_t currentFrame) override;
	virtual void Draw(VkCommandBuffer commandBuffer) override;

	virtual void LinkMaterial(Material* pMaterial) override { m_pMaterial = pMaterial; }

	void AddVertex(T vertex);

	//void AddTriangle(TrianglePosCol triangle, bool updateBuffer = false);
	//void AddTriangle(PosCol2D vertex1, PosCol2D vertex2, PosCol2D vertex3, bool updateBuffer = false);
	//
	//void AddQuad(QuadPosCol quad, bool updateBuffer = false);
	//void AddQuad(PosCol2D vertex1, PosCol2D vertex2, PosCol2D vertex3, PosCol2D vertex4, bool updateBuffer = false);

protected:
	uint32_t m_VertexCapacity;
	bool m_Is2D;
	std::vector<T> m_Vertices{};
	Material* m_pMaterial;

	// Vertex buffer
	VkBuffer m_VertexBuffer{ nullptr }, m_StagingBuffer{ nullptr };
	VkDeviceMemory m_VertexBufferMemory{ nullptr }, m_StagingBufferMemory{ nullptr };

	// Uniform (Constant) buffer
	std::vector<VkBuffer> m_UniformBuffers{ nullptr };
	std::vector<VkDeviceMemory> m_UniformBuffersMemory{ nullptr };
	std::vector<void*> m_UniformBuffersMapped{ nullptr };

	// TODO: Make separate like command pool? Maybe at the pipeline??
	VkDescriptorPool m_DescriptorPool;
	std::vector<VkDescriptorSet> m_DescriptorSets;

	void CreateBuffer(const GameContext& context, VkDeviceSize size, VkBufferUsageFlags usage,
	                  VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void CreateVertexBuffer(const GameContext& context);
	void CreateUniformBuffers(const GameContext& context);

	void CreateDescriptorPool(const GameContext& context);
	void CreateDescriptorSets(const GameContext& context);

	void UpdateVertexBuffer(const GameContext& context, VkDeviceSize size, VkDeviceMemory bufferMemory);
	void CopyBuffer(const GameContext& context, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

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

template <typename T>
Mesh<T>::Mesh(uint32_t vertexCapacity, bool is2d)
	: m_VertexCapacity(vertexCapacity)
	, m_Is2D(is2d)

{
}

template <class T>
void Mesh<T>::Init(const GameContext& context)
{
	CreateVertexBuffer(context);
	CreateUniformBuffers(context);

	if (m_Is2D == false)
	{
		CreateDescriptorPool(context);
		CreateDescriptorSets(context);
	}
}

template <class T>
void Mesh<T>::CleanUp(const GameContext& context)
{
	for (size_t i = 0; i < m_UniformBuffers.size(); i++)
	{
		vkDestroyBuffer(context.vulkanContext.device, m_UniformBuffers[i], nullptr);
		vkFreeMemory(context.vulkanContext.device, m_UniformBuffersMemory[i], nullptr);
	}

	vkDestroyBuffer(context.vulkanContext.device, m_VertexBuffer, nullptr);
	vkFreeMemory(context.vulkanContext.device, m_VertexBufferMemory, nullptr);

	vkDestroyBuffer(context.vulkanContext.device, m_StagingBuffer, nullptr);
	vkFreeMemory(context.vulkanContext.device, m_StagingBufferMemory, nullptr);

	if (m_Is2D == false)
		vkDestroyDescriptorPool(context.vulkanContext.device, m_DescriptorPool, nullptr);
}

template <class T>
void Mesh<T>::Update(uint32_t currentFrame)
{
	if (m_Is2D)
		return;

	UniformBufferObject ubo{};
	const auto time = GameTime::GetInstance().GetElapsed();
	ubo.model = glm::rotate(glm::mat4(1.f), glm::radians(0.f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj = Camera::GetInstance().GetProjection();
	ubo.proj[1][1] *= -1;
	ubo.view = Camera::GetInstance().GetView();

	memcpy(m_UniformBuffersMapped[currentFrame], &ubo, sizeof(UniformBufferObject));

	vkCmdBindDescriptorSets(CommandPool::GetInstance().GetCommandBuffer(currentFrame),
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		m_pMaterial->GetPipeline()->GetPipelineLayout(),
		0,
		1,
		&m_DescriptorSets[currentFrame],
		0,
		nullptr
	);
}

template <class T>
void Mesh<T>::Draw(VkCommandBuffer commandBuffer)
{
	VkBuffer vertexBuffers[] = { m_VertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

	vkCmdDraw(commandBuffer, static_cast<uint32_t>(m_Vertices.size()), 1, 0, 0);
}

template <class T>
void Mesh<T>::AddVertex(T vertex)
{
	m_Vertices.push_back(vertex);
}

template <class T>
void Mesh<T>::CreateBuffer(const GameContext& context, VkDeviceSize size, VkBufferUsageFlags usage,
	VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
	// Create buffer
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(context.vulkanContext.device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(context.vulkanContext.device, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = FindMemoryType(context, memRequirements.memoryTypeBits, properties);

	// TODO: This could be improved -> see Conclusion @ https://vulkan-tutorial.com/Vertex_buffers/Staging_buffer
	if (vkAllocateMemory(context.vulkanContext.device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate buffer memory!");
	}

	vkBindBufferMemory(context.vulkanContext.device, buffer, bufferMemory, 0);
}

template <class T>
void Mesh<T>::CreateVertexBuffer(const GameContext& context)
{
	const VkDeviceSize bufferSize = sizeof(T) * m_Vertices.size();
#pragma region Staging Buffer

	CreateBuffer(context, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		m_StagingBuffer, m_StagingBufferMemory);

	UpdateVertexBuffer(context, bufferSize, m_StagingBufferMemory);

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
	//	UpdateVertexBuffer(context, bufferSize, m_VertexBufferMemory);
	//#pragma endregion
}

template <class T>
void Mesh<T>::CreateUniformBuffers(const GameContext& context)
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

template <class T>
void Mesh<T>::CreateDescriptorPool(const GameContext& context)
{
	VkDescriptorPoolSize poolSize{};
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &poolSize;
	poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

	if (vkCreateDescriptorPool(context.vulkanContext.device, &poolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

template <class T>
void Mesh<T>::CreateDescriptorSets(const GameContext& context)
{
	std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, m_pMaterial->GetPipeline()->GetDescriptorSetLayout());
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
		bufferInfo.buffer = m_UniformBuffers[i];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = m_DescriptorSets[i];
		descriptorWrite.dstBinding = 0;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;

		vkUpdateDescriptorSets(context.vulkanContext.device, 1, &descriptorWrite, 0, nullptr);
	}
}

template <class T>
void Mesh<T>::UpdateVertexBuffer(const GameContext& context, VkDeviceSize size, VkDeviceMemory bufferMemory)
{
	void* data;
	vkMapMemory(context.vulkanContext.device, bufferMemory, 0, size, 0, &data);
	memcpy(data, m_Vertices.data(), sizeof(T) * m_Vertices.size());
	vkUnmapMemory(context.vulkanContext.device, bufferMemory);
}

template <class T>
void Mesh<T>::CopyBuffer(const GameContext& context, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
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


#endif // MESH_H