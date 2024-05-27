#ifndef GAMESTRUCTS_H
#define GAMESTRUCTS_H

#include <vulkan/vulkan_core.h>

#include <glm/matrix.hpp>
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

struct WVP_Time
{
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
	int index;
};

struct WorldMatrix
{
	glm::mat4 model;
};

struct PosTex
{
	glm::vec3 pos;
	glm::vec2 texCoord;

	static constexpr size_t binding_count = 1;
	static std::array<VkVertexInputBindingDescription, binding_count> GetBindingDescription()
	{
		std::array<VkVertexInputBindingDescription, binding_count> bindingDescriptions{};

		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(PosTex);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescriptions;
	}

	static constexpr size_t attribute_count = 2; // Define the size of the attribute array
	static std::array<VkVertexInputAttributeDescription, attribute_count> GetAttributeDescriptions()
	{
		std::array<VkVertexInputAttributeDescription, attribute_count> attributeDescriptions{};

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(PosTex, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(PosTex, texCoord);

		return attributeDescriptions;
	}
};

struct GuiElement
{
	glm::vec4 uv;
	glm::vec2 position;
	std::array<PosTex, 4> vertices;
};

#endif // GAMESTRUCTS_H