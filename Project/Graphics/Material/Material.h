#ifndef MATERIAL_H
#define MATERIAL_H

#include <vulkan/vulkan_core.h>

#include "Util/Logger.h"

#include "Pipelines/Pipeline.h"
#include "RenderPasses/RenderPass.h"
#include "CommandBuffers/CommandBuffer.h"
#include "Core/SwapChain.h"

class Material final
{
public:
	explicit Material() = default;
	~Material() = default;

	Material(const Material&) = delete;
	Material operator=(const Material&) = delete;
	Material(Material&&) = delete;
	Material operator=(Material&&) = delete;

	template <typename T>
	void AddPipeline(const GameContext& context);
	template <typename T>
	void AddRenderPass(const GameContext& context, const SwapChain& swapChain);
	template <typename T>
	void AddCommandBuffer(const GameContext& context);

	Pipeline* GetPipeline() const { return m_pPipeline.get(); }
	CommandBuffer* GetCommandBuffer() const { return m_pBuffer.get(); }
	RenderPass* GetRenderPass() const { return m_pRenderPass.get(); }

	void RecordCommandBuffer(const SwapChain& swapChain, const VkFramebuffer& frameBuffer);

private:
	std::unique_ptr<Pipeline> m_pPipeline{ nullptr };
	std::unique_ptr<CommandBuffer> m_pBuffer{ nullptr };
	std::unique_ptr<RenderPass> m_pRenderPass{ nullptr };

	void DrawFrame(const SwapChain& swapChain, const VkFramebuffer& frameBuffer);
};

template <typename T>
void Material::AddPipeline(const GameContext& context)
{
	if constexpr (std::is_base_of_v<Pipeline, T> == false)
	{
		Logger::LogError({ "T must derive from Pipeline" });
		return;
	}

	if (m_pRenderPass == nullptr)
	{
		Logger::LogError({ "A render pass must first be created before a pipeline" });
		return;
	}

	m_pPipeline = std::make_unique<T>();
	m_pPipeline->CreatePipeline(context.vulkanContext.device, m_pRenderPass.get());
}

template <typename T>
void Material::AddRenderPass(const GameContext& context, const SwapChain& swapChain)
{
	if constexpr (std::is_base_of_v<RenderPass, T> == false)
	{
		Logger::LogError({ "T must derive from RenderPass" });
		return;
	}

	m_pRenderPass = std::make_unique<T>();
	m_pRenderPass->Create(context.vulkanContext.device, swapChain.GetFormat());
}

template <typename T>
void Material::AddCommandBuffer(const GameContext& context)
{
	if constexpr (std::is_base_of_v<CommandBuffer, T> == false)
	{
		Logger::LogError({ "T must derive from CommandBuffer" });
		return;
	}

	m_pBuffer = std::make_unique<T>(context);
}

#endif // MATERIAL_H
