#ifndef MESH_H
#define MESH_H

#include <iomanip>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <vulkan/vulkan_core.h>

#include "Misc/Camera.h"

#include "Core/CommandPool.h"
#include "Graphics/Renderer.h"

#include "Util/Concepts.h"
#include "Util/Structs.h"
#include "Util/GameTime.h"
#include "Util/VulkanUtil.h"

template <vertex_type V>
class Mesh
{
public:
	explicit Mesh(uint32_t vertexCapacity, bool is2d)
		: m_VertexCapacity(vertexCapacity)
		, m_Is2D(is2d)
	{
	}

	virtual ~Mesh() = default;

	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;
	Mesh(Mesh&&) = delete;
	Mesh& operator=(Mesh&&) = delete;

	virtual void Init(const GameContext& context)
	{
		CreateVertexBuffer(context);
	}
	virtual void CleanUp(const GameContext& context)
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

	void CreateDescriptor(const GameContext& context, VkDescriptorSetLayout layout)
	{
		if (m_Is2D)
			return;

		CreateUniformBuffers(context);
		CreateDescriptorPool(context);
		CreateDescriptorSets(context, layout);
	}

	void Update(uint32_t currentFrame, VkCommandBuffer buffer, VkPipelineLayout layout)
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
	uint32_t m_VertexCapacity;
	bool m_Is2D;
	std::vector<V> m_Vertices{};

	// Vertex buffer
	VkBuffer m_VertexBuffer{ nullptr }, m_StagingBuffer{ nullptr };
	VkDeviceMemory m_VertexBufferMemory{ nullptr }, m_StagingBufferMemory{ nullptr };

	// Uniform (Constant) buffer
	std::vector<VkBuffer> m_UniformBuffers{ nullptr };
	std::vector<VkDeviceMemory> m_UniformBuffersMemory{ nullptr };
	std::vector<void*> m_UniformBuffersMapped{ nullptr };

	// TODO: Make separate like command pool? Maybe at the pipeline??
	VkDescriptorPool m_DescriptorPool{};
	std::vector<VkDescriptorSet> m_DescriptorSets{};

	static void CreateBuffer(const GameContext& context, VkDeviceSize size, VkBufferUsageFlags usage,
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

	void UpdateVertexBuffer(const GameContext& context, VkDeviceSize size, VkDeviceMemory bufferMemory)
	{
		void* data;
		vkMapMemory(context.vulkanContext.device, bufferMemory, 0, size, 0, &data);
		memcpy(data, m_Vertices.data(), sizeof(V) * m_Vertices.size());
		vkUnmapMemory(context.vulkanContext.device, bufferMemory);
	}

	void CopyBuffer(const GameContext& context, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
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


#endif // MESH_H