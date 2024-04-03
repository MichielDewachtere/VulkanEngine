#ifndef MATERIAL_H
#define MATERIAL_H

#include "Pipelines/Pipeline.h"
#include "Core/SwapChain.h"

#include "Util/Logger.h"

class BaseMesh;
class SwapChain;

class Material final
{
public:
	explicit Material() = default;
	~Material() = default;

	Material(const Material&) = delete;
	Material operator=(const Material&) = delete;
	Material(Material&&) = delete;
	Material operator=(Material&&) = delete;

	void CleanUp(const GameContext& context);

	template <typename tPipeline>
		requires std::is_base_of_v<Pipeline, tPipeline>
	void AddPipeline(const GameContext& context);

	void BindMesh(BaseMesh* pMesh);

	Pipeline* GetPipeline() const { return m_pPipeline.get(); }
	void DrawFrame(const GameContext& context) const;

	bool IsActive() const { return m_IsActive; }
	void SetIsActive(bool isActive) { m_IsActive = isActive; }

private:
	bool m_IsActive{ true };
	std::unique_ptr<Pipeline> m_pPipeline{ nullptr };
	std::vector<BaseMesh*> m_pMeshes;
};

template <typename tPipeline>
	requires std::is_base_of_v<Pipeline, tPipeline>
void Material::AddPipeline(const GameContext& context)
{
	m_pPipeline = std::make_unique<tPipeline>();
	m_pPipeline->CreatePipeline(context.vulkanContext);
}
#endif // MATERIAL_H