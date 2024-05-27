#include "DescriptorPoolManager.h"

#include <stdexcept>

#include "RealEngine.h"

VkDescriptorSet real::DescriptorPoolManager::AllocateDescriptorSet(VkDescriptorSetLayout layout)
{
    const auto context = RealEngine::GetGameContext();

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_CurrentPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &layout;

    VkDescriptorSet descriptorSet;
    if (vkAllocateDescriptorSets(context.vulkanContext.device, &allocInfo, &descriptorSet) == VK_SUCCESS) 
    {
        return descriptorSet;
    }

    // If allocation failed, create a new pool and try again
    AllocateNewPool();
    allocInfo.descriptorPool = m_CurrentPool;
    if (vkAllocateDescriptorSets(context.vulkanContext.device, &allocInfo, &descriptorSet) != VK_SUCCESS) 
    {
        throw std::runtime_error("failed to allocate descriptor set");
    }

    return descriptorSet;
}

void real::DescriptorPoolManager::CleanUp()
{
    const auto context = RealEngine::GetGameContext();

    for (const auto pool : m_DescriptorPools) 
    {
        vkDestroyDescriptorPool(context.vulkanContext.device, pool, nullptr);
    }
    m_DescriptorPools.clear();
}

real::DescriptorPoolManager::DescriptorPoolManager()
    : m_CurrentPool(VK_NULL_HANDLE)
	, m_CurrentPoolSize(0)
{
    AllocateNewPool();
}

VkDescriptorPool real::DescriptorPoolManager::CreateDescriptorPool(uint32_t size)
{
    const auto context = RealEngine::GetGameContext();

    const std::vector<VkDescriptorPoolSize> poolSizes = {
         { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, size },
         { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, size },
    };

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = size;

    VkDescriptorPool descriptorPool;
    if (vkCreateDescriptorPool(context.vulkanContext.device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool");
    }

    return descriptorPool;
}

void real::DescriptorPoolManager::AllocateNewPool()
{
    m_CurrentPoolSize = (m_DescriptorPools.empty()) ? m_InitialPoolSize : static_cast<uint32_t>(m_CurrentPoolSize * m_GrowthFactor);
    m_CurrentPool = CreateDescriptorPool(m_CurrentPoolSize);
    m_DescriptorPools.push_back(m_CurrentPool);
}