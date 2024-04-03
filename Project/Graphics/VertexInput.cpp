#include "VertexInput.h"

VertexInput::VertexInput()
{
	m_VertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	m_VertexInputInfo.vertexBindingDescriptionCount = 0;
	m_VertexInputInfo.vertexAttributeDescriptionCount = 0;
	//m_VertexInputInfo.flags = NULL;
}

VkPipelineVertexInputStateCreateInfo VertexInput::GetInfo()
{
	m_VertexInputInfo.pVertexAttributeDescriptions = m_AttributeDescriptions.data();
	m_VertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(m_AttributeDescriptions.size());

	m_VertexInputInfo.pVertexBindingDescriptions = m_BindingDescriptions.data();
	m_VertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(m_BindingDescriptions.size());

	return m_VertexInputInfo;
}

void VertexInput::AddAttribute(const VkFormat format, const uint32_t size)
{
	VkVertexInputAttributeDescription attributeDesc;
	attributeDesc.binding = 0;
	attributeDesc.location = m_AttributeIndex++;
	attributeDesc.format = format;
	attributeDesc.offset = m_Offset;
	m_Offset += size;

	m_AttributeDescriptions.push_back(attributeDesc);
}

void VertexInput::AddBinding(uint32_t size, bool instancing)
{
	VkVertexInputBindingDescription bindingDesc;
	bindingDesc.binding = m_BindingIndex++;
	bindingDesc.stride = size;
	if (instancing)
		bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
	else
		bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	m_BindingDescriptions.push_back(bindingDesc);
}

VkPipelineInputAssemblyStateCreateInfo VertexInput::CreateInputAssemblyStateInfo(VkPrimitiveTopology topology)
{
	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = topology;
	if (topology == VK_PRIMITIVE_TOPOLOGY_LINE_STRIP
		|| topology == VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP
		|| topology == VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY
		|| topology == VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY)
		inputAssembly.primitiveRestartEnable = VK_TRUE;
	else
		inputAssembly.primitiveRestartEnable = VK_FALSE;

	return inputAssembly;
}
