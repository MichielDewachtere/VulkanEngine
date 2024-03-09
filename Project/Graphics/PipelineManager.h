#ifndef PIPELINEMANAGER_H
#define PIPELINEMANAGER_H

#include <vector>

#include "Util/Singleton.h"
#include "Util/VulkanUtil.h"

class RenderPass;
class Pipeline;

struct PipelineInfo
{
	UINT8 id, renderPass;
	std::string name;
	Pipeline* pipeline{ nullptr };
	// TODO: add CommandPool and CommandBuffer?
};

struct RenderPassInfo
{
	UINT8 id;
	std::string name;
	RenderPass* renderPass;
};

class PipelineManager final : public Singleton<PipelineManager>
{
public:
	virtual ~PipelineManager() override = default;

	UINT8 AddPipeline(VkDevice device, Pipeline* pPipeline, UINT8 renderPass, std::string name = std::string(""));
	Pipeline* GetPipeline(const std::string& name) const;
	Pipeline* GetPipeline(UINT8 id) const;

	UINT8 AddRenderPass(VkDevice device, RenderPass* pRenderPass, VkFormat format, std::string name = std::string(""));
	RenderPass* GetRenderPass(const std::string& name) const;
	RenderPass* GetRenderPass(UINT8 id) const;

	//void AddCommandBuffer(VkDevice device, UINT8 pipeline);
	//void RecordCommandBuffer(VkDevice device, UINT8 pipeline, uint32_t imageIndex);

	void CleanUp(VkDevice device);

private:
	friend class Singleton<PipelineManager>;
	PipelineManager() = default;

	std::vector<PipelineInfo> m_Pipelines{};
	std::vector<RenderPassInfo> m_RenderPasses{};

	void CreateCommandPool(UINT8 id);
	void DrawFrame(UINT8 id, uint32_t imageIndex);

	template <typename T>
	T GetInfo(const std::vector<T>& v, const std::string& name) const;
};

template <typename T>
[[nodiscard]] T PipelineManager::GetInfo(const std::vector<T>& v, const std::string& name) const
{
	const auto it = std::find_if(v.begin(), v.end(), [&](const T& p)
		{
			if (p.name == name)
				return true;

			return false;
		});

	if (it == v.end())
		throw std::runtime_error("No info found with name: " + name);

	return *it;
}

#endif // PIPELINEMANAGER_H
