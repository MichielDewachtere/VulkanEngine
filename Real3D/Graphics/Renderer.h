#ifndef RENDERER_H
#define RENDERER_H

#include <real_core/Singleton.h>

#include "Util/Structs.h"


namespace real
{
	class SwapChain;

	class Renderer final : public real::Singleton<Renderer>
	{
	public:
		virtual ~Renderer() override = default;

		Renderer(const Renderer&) = delete;
		Renderer operator=(const Renderer&) = delete;
		Renderer(Renderer&&) = delete;
		Renderer operator=(Renderer&&) = delete;

		void Init(GameContext& context);  // NOLINT(clang-diagnostic-overloaded-virtual)
		void CleanUp(const GameContext& context);

		void Draw(const GameContext& context);

		SwapChain* GetSwapChain() const { return m_pSwapChain; }

		uint32_t GetCurrentFrame() const { return m_CurrentFrame; }
		VkFramebuffer GetCurrentFrameBuffer() const { return m_SwapChainFrameBuffers[m_CurrentFrame]; }

		VkQueue GetGraphicsQueue() const { return m_GraphicsQueue; }
		VkQueue GetPresentQueue() const { return m_PresentQueue; }

	private:
		friend class Singleton<Renderer>;
		Renderer() = default;

		uint32_t m_CurrentFrame{ 0 };
		VkQueue m_GraphicsQueue{ nullptr }, m_PresentQueue{ nullptr };
		std::vector<VkSemaphore> m_ImageAvailableSemaphores, m_RenderFinishedSemaphores;
		std::vector<VkFence> m_InFlightFences;
		SwapChain* m_pSwapChain;
		std::vector<VkFramebuffer> m_SwapChainFrameBuffers;

		void CreateFrameBuffers(const GameContext& context);
		void CreateSyncObjects(const GameContext& context);
	};
}

#endif // RENDERER_H
