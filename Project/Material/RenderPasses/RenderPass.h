#ifndef RENDERPASS_H
#define RENDERPASS_H

#include "Util/VulkanUtil.h"

class RenderPass
{
public:
	RenderPass() = default;
	virtual ~RenderPass() = default;

	RenderPass(const RenderPass&) = delete;
	RenderPass& operator=(const RenderPass&) = delete;
	RenderPass(RenderPass&&) = delete;
	RenderPass& operator=(RenderPass&&) = delete;

	virtual void Create(VkDevice device, VkFormat format) = 0;
	VkRenderPass GetRenderPass() const { return m_RenderPass; }

	void CleanUp(VkDevice device);

protected:
	bool m_IsCreated{ false };
	VkRenderPass m_RenderPass;
};

#endif // RENDERPASS_H
