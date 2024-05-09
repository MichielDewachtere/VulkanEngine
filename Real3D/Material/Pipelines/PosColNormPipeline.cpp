#include "PosColNormPipeline.h"

#include "Util/VulkanUtil.h"

void real::PosColNormPipeline::CleanUp(VkDevice device)
{
	Pipeline::CleanUp(device);
}

void real::PosColNormPipeline::CreatePipeline(const VulkanContext& vulkan)
{
	CreateDescriptorSetLayout(vulkan.device);

	SetVertShader("PosColNorm.vert.spv");
	SetFragShader("PosColNorm.frag.spv");

	SetPrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

	PosNormBase::CreatePipeline(vulkan);
}

void real::PosColNormPipeline::CreateDescriptorSetLayout(const VkDevice& device)
{
	VkDescriptorSetLayoutBinding uboLayoutBinding;
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings = &uboLayoutBinding;

	if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &m_DescriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}
