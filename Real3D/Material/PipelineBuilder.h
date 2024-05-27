#ifndef PIPELINEBUILDER_H
#define PIPELINEBUILDER_H

#include <vector>
#include <vulkan/vulkan.h>

#include "PipelineEnums.h"
#include "Util/Concepts.h"

namespace real
{
	class PipelineBuilder final
	{
	public:
		explicit PipelineBuilder(VkDevice device);
		~PipelineBuilder() = default;

		PipelineBuilder(const PipelineBuilder&) = delete;
		PipelineBuilder& operator=(const PipelineBuilder&) = delete;
		PipelineBuilder(PipelineBuilder&&) = delete;
		PipelineBuilder& operator=(PipelineBuilder&&) = delete;

		void SetShaders(std::vector<VkPipelineShaderStageCreateInfo> shaderModules);
		void SetInputAssembly(EPrimitiveTopology topology, bool primitiveRestart);
		template<vertex_type V>
		void SetInputType();
		void SetRasterizer(ERenderMode renderMode, ECullMode cullMode, float lineWidth = 1.0f);
		void SetColorBlend();

	private:
		VkDevice m_Device;
	};

}

#endif // PIPELINEBUILDER_H