#ifndef DEFAULTRENDERPASS_H
#define DEFAULTRENDERPASS_H

#include "RenderPass.h"

class DefaultRenderPass final : public RenderPass
{
public:
	DefaultRenderPass() = default;
	~DefaultRenderPass() override = default;

	virtual void Create(VkDevice device, VkFormat format) override;
};

#endif // DEFAULTRENDERPASS_H