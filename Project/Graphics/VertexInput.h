#ifndef VERTEXINPUT_H
#define VERTEXINPUT_H

#include "Util/VulkanUtil.h"

class VertexInput final
{
public:
	VertexInput();
	~VertexInput() = default;

	VkPipelineVertexInputStateCreateInfo GetInfo();

	/**
	 * \brief Add an attribute to the VertexInput
	 * \param format The format of the attribute, see vulkan_core.h line 1492
	 * \param size the size of the format in bytes
	 */
	void AddAttribute(VkFormat format, uint32_t size);
	template <typename T>
	void AddBinding(bool instancing = false);
	void AddBinding(uint32_t size, bool instancing = false);

	static VkPipelineInputAssemblyStateCreateInfo CreateInputAssemblyStateInfo(VkPrimitiveTopology topology);

private:
	uint32_t m_AttributeIndex{ 0 }, m_Offset{ 0 };
	uint32_t m_BindingIndex{ 0 };
	std::vector<VkVertexInputAttributeDescription> m_AttributeDescriptions{};
	std::vector<VkVertexInputBindingDescription> m_BindingDescriptions{};
	VkPipelineVertexInputStateCreateInfo m_VertexInputInfo{};
};

template <typename T>
void VertexInput::AddBinding(bool instancing)
{
	AddBinding(sizeof(T), instancing);
}

#endif // VERTEXINPUT_H
