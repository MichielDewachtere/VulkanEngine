#include "Material.h"

#include <stdexcept>

#include "Core/CommandPool.h"
#include "Graphics/Renderer.h"
#include "Mesh/BaseMesh.h"

void Material::CleanUp(const GameContext& context)
{
	m_pPipeline->CleanUp(context.vulkanContext.device);
}

void Material::BindMesh(BaseMesh* pMesh)
{
	m_pMeshes.push_back(pMesh);
	m_pMeshes.back()->LinkMaterial(this);
}

void Material::DrawFrame(const GameContext& context) const
{
	auto swapChainExtent = Renderer::GetInstance().GetSwapChain()->GetExtent();

	const auto currentFrame = Renderer::GetInstance().GetCurrentFrame();
	const auto commandBuffer = CommandPool::GetInstance().GetCommandBuffer(currentFrame);

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pPipeline->GetPipeline());

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)swapChainExtent.width;
	viewport.height = (float)swapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = swapChainExtent;
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	// DrawScene();
	for (const auto & m : m_pMeshes)
	{
		m->Update(currentFrame);
		m->Draw(commandBuffer);
	}
}
