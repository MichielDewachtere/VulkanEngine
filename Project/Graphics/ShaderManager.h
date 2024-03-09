#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

#include <map>
#include <string>

#include "Util/Singleton.h"
#include "Util/VulkanUtil.h"

enum class ShaderType
{
	vertex = 0,
	fragment = 1,
	geometry = 2,
	//tessellation = 3,
};

class ShaderManager final : public Singleton<ShaderManager>
{
public:
	void Init(std::string shaderSourcePath);  // NOLINT(clang-diagnostic-overloaded-virtual)
	virtual ~ShaderManager() override = default;

	VkPipelineShaderStageCreateInfo CreateShaderInfo(const VkDevice& device, ShaderType type, const std::string&
	                                                 fileName, const char* entryPoint = "main");

	void DestroyShaderModules(VkDevice device);

private:
	std::string m_ShaderDataPath{};
	std::map<std::string, VkPipelineShaderStageCreateInfo> m_ShaderPtrs;

	friend class Singleton<ShaderManager>;
	ShaderManager() = default;

	static VkShaderModule CreateShaderModule(const VkDevice& device, const std::vector<char>& code);

	const inline static std::map<ShaderType, VkShaderStageFlagBits> type_to_shader_stage
	{
		{ShaderType::vertex, VK_SHADER_STAGE_VERTEX_BIT},
		{ShaderType::fragment, VK_SHADER_STAGE_FRAGMENT_BIT},
		{ShaderType::geometry, VK_SHADER_STAGE_GEOMETRY_BIT},
	};
};

#endif // SHADERMANAGER_H