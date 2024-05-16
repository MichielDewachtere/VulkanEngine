#ifndef MATERIAL_H
#define MATERIAL_H

#include <real_core/Observer.h>

#include "Pipelines/Pipeline.h"
#include "Core/CommandPool.h"
#include "Util/Concepts.h"
#include "Mesh/Mesh.h"
#include "BaseMaterial.h"

namespace real
{
	class SwapChain;

	template <pipeline_type P, vertex_type V>
	class Material final
		: public BaseMaterial
		, public Observer<GameObjectEvent, GameObject*>
	{
	public:
		explicit Material(const VulkanContext& context)
		{
			m_pPipeline = std::make_unique<P>();
			m_pPipeline->CreatePipeline(context);
		}

		virtual ~Material() override = default;

		Material(const Material&) = delete;
		Material& operator=(const Material&) = delete;
		Material(Material&&) = delete;
		Material& operator=(Material&&) = delete;

		virtual void CleanUp(const GameContext& context) override
		{
			m_pPipeline->CleanUp(context.vulkanContext.device);
		}

		void BindMesh(const GameContext& context, Mesh<V>* pMesh)
		{
			m_pMeshes.push_back(pMesh);
			pMesh->GetOwner()->gameObjectDestroyed.AddObserver(this);

			if (auto layout = m_pPipeline->GetDescriptorSetLayout();
				layout != VK_NULL_HANDLE)
					m_pMeshes.back()->CreateDescriptor(context, layout);
		}

		Pipeline* GetPipeline() const { return m_pPipeline.get(); }

		virtual void DrawFrame(uint32_t currentFrame, VkExtent2D extent) const override
		{
			const auto commandBuffer = CommandPool::GetInstance().GetCommandBuffer(currentFrame);

			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pPipeline->GetPipeline());

			VkViewport viewport{};
			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = (float)extent.width;
			viewport.height = (float)extent.height;
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
			vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

			VkRect2D scissor{};
			scissor.offset = { 0, 0 };
			scissor.extent = extent;
			vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

			// DrawScene();
			for (const auto& m : m_pMeshes)
			{
				m->UpdateUbo(currentFrame, commandBuffer, m_pPipeline->GetPipelineLayout());
				m->Draw(commandBuffer);
			}
		}

		void HandleEvent(GameObjectEvent, GameObject*) override;
		void OnSubjectDestroy() override {}

	private:
		std::unique_ptr<Pipeline> m_pPipeline{ nullptr };
		std::vector<Mesh<V>*> m_pMeshes;
	};

	template <pipeline_type P, vertex_type V>
	void Material<P, V>::HandleEvent(GameObjectEvent event, GameObject* go)
	{
		if (event == GameObjectEvent::destroyed)
		{
			go->gameObjectDestroyed.RemoveObserver(this);

			auto pMesh = go->GetComponent<Mesh<V>>();
			const auto it = std::ranges::find(m_pMeshes, pMesh);
			if (it == m_pMeshes.end())
				return;

			m_pMeshes.erase(it);
		}
	}
}

#endif // MATERIAL_H