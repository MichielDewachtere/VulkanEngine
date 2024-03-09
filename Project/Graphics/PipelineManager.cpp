#include "PipelineManager.h"

#include <algorithm>

#include "Material/Pipelines/Pipeline.h"
#include "Material/RenderPasses/RenderPass.h"

UINT8 PipelineManager::AddPipeline(const VkDevice device, Pipeline* pPipeline, UINT8 renderPass, std::string name)
{
	PipelineInfo info;
	info.id = static_cast<UINT8>(m_Pipelines.size());
	info.name = std::move(name);
	info.pipeline = pPipeline;
	info.renderPass = renderPass;

	if (info.pipeline->IsCreated() == false)
	{
		const auto pRenderPass = m_RenderPasses[renderPass].renderPass;
		info.pipeline->CreatePipeline(device, pRenderPass);
	}

	m_Pipelines.push_back(info);
	return info.id;
}

Pipeline* PipelineManager::GetPipeline(const std::string& name) const
{
	const auto info = GetInfo<PipelineInfo>(m_Pipelines, name);
	return info.pipeline;
}

Pipeline* PipelineManager::GetPipeline(UINT8 id) const
{
	return m_Pipelines[id].pipeline;
}

UINT8 PipelineManager::AddRenderPass(const VkDevice device, RenderPass* pRenderPass, VkFormat format, std::string name)
{
	RenderPassInfo info;
	info.id = static_cast<UINT8>(m_Pipelines.size());
	info.name = std::move(name);
	info.renderPass = pRenderPass;
	info.renderPass->Create(device, format);

	m_RenderPasses.push_back(info);
	return info.id;
}

RenderPass* PipelineManager::GetRenderPass(const std::string& name) const
{
	const auto info = GetInfo<RenderPassInfo>(m_RenderPasses, name);
	return info.renderPass;
}

RenderPass* PipelineManager::GetRenderPass(UINT8 id) const
{
	return m_RenderPasses[id].renderPass;
}

void PipelineManager::CleanUp(VkDevice device)
{
	std::for_each(m_Pipelines.begin(), m_Pipelines.end(), [&](PipelineInfo p)
		{
			if (p.pipeline != nullptr)
			{
				p.pipeline->CleanUp(device);
				delete p.pipeline;
				p.pipeline = nullptr;
			}
		});

	m_Pipelines.clear();
	std::for_each(m_RenderPasses.begin(), m_RenderPasses.end(), [&](RenderPassInfo p)
		{
			if (p.renderPass != nullptr)
			{
				p.renderPass->CleanUp(device);
				delete p.renderPass;
				p.renderPass = nullptr;
			}
		});
	m_RenderPasses.clear();
}
