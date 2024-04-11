#include "ShaderManager.h"

#include <algorithm>
#include <iostream>
#include <ranges>

void ShaderManager::Init(std::string shaderSourcePath)
{
	m_ShaderDataPath = std::move(shaderSourcePath);
}

VkPipelineShaderStageCreateInfo ShaderManager::CreateShaderInfo(const VkDevice& device, const ShaderType type,
                                                                const std::string& fileName,
                                                                const char* entryPoint)
{
	if (m_ShaderPtrs.find(fileName) != m_ShaderPtrs.end())
		return m_ShaderPtrs[fileName];

	const auto filepath = m_ShaderDataPath + '/' + fileName;
	const std::vector<char> shaderCode = readFile(filepath);
	const VkShaderModule shaderModule = CreateShaderModule(device, shaderCode);

	VkPipelineShaderStageCreateInfo shaderStageInfo{};
	shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStageInfo.stage = type_to_shader_stage.at(type);
	shaderStageInfo.module = shaderModule;
	shaderStageInfo.pName = entryPoint;

	m_ShaderPtrs[fileName] = shaderStageInfo;
	return m_ShaderPtrs[fileName];
}

void ShaderManager::DestroyShaderModules(VkDevice device)
{
	for (const auto& shader : m_ShaderPtrs | std::views::values)
	{
		vkDestroyShaderModule(device, shader.module, nullptr);
	}

	m_ShaderPtrs.clear();
}

VkShaderModule ShaderManager::CreateShaderModule(const VkDevice& device, const std::vector<char>& code)
{
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to create shader module!");
	}

	return shaderModule;
}
