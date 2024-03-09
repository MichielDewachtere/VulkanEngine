#ifndef RENDERER_H
#define RENDERER_H

#include "Material/Material.h"
#include "Util\Structs.h"


class SwapChain;

class Renderer final
{
public:
	explicit Renderer() = default;
	~Renderer() = default;

	Renderer(const Renderer&) = delete;
	Renderer operator=(const Renderer&) = delete;
	Renderer(Renderer&&) = delete;
	Renderer operator=(Renderer&&) = delete;

	void Init(const GameContext& context);
	void CleanUp(const GameContext& context);

	void CreateFrameBuffers(const GameContext& context);
	void CreateSyncObjects(const GameContext& context);

	void AddMaterial(Material* pMaterial);
	SwapChain* GetSwapChain() const { return m_pSwapChain; }

	void Draw(const GameContext& context);

private:
	VkQueue m_GraphicsQueue, m_PresentQueue;
	VkSemaphore m_ImageAvailableSemaphore, m_RenderFinishedSemaphore;
	VkFence m_InFlightFence;
	SwapChain* m_pSwapChain;

	std::vector<VkFramebuffer> m_SwapChainFrameBuffers;

	Material* m_pMaterial{ nullptr };
	//std::vector<Material*> m_pMaterials{};
};

#endif // RENDERER_H