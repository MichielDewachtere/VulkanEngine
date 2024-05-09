#ifndef POSCOL2DPIPELINE_H
#define POSCOL2DPIPELINE_H

#include "Pipeline.h"

namespace real
{
	class PosCol2DPipeline final : public Pipeline
	{
	public:
		PosCol2DPipeline() = default;
		virtual ~PosCol2DPipeline() override = default;

		virtual void CleanUp(VkDevice device) override;

		virtual void CreatePipeline(const VulkanContext& vulkan) override;
	};
}

#endif // POSCOL2DPIPELINE_H