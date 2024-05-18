#ifndef	TRANSPARENCYPIPELINE_H
#define TRANSPARENCYPIPELINE_H

#include <Material/Pipelines/Pipeline.h>

class TransparencyPipeline : public real::Pipeline
{
public:
	TransparencyPipeline() = default;
	virtual ~TransparencyPipeline() override = default;

	TransparencyPipeline(const TransparencyPipeline&) = delete;
	TransparencyPipeline& operator=(const TransparencyPipeline&) = delete;
	TransparencyPipeline(TransparencyPipeline&&) = delete;
	TransparencyPipeline& operator=(TransparencyPipeline&&) = delete;

	virtual void CreatePipeline(const real::VulkanContext& vulkan) override;

protected:
	virtual void CreateDescriptorSetLayout(const VkDevice& device) = 0;

	void SetVertShader(const std::string& path) { m_VertShader = path; }
	void SetFragShader(const std::string& path) { m_FragShader = path; }

	void SetPrimitiveTopology(VkPrimitiveTopology topology) { m_PrimitiveTopology = topology; }

private:
	std::string m_VertShader{}, m_FragShader{};
	VkPrimitiveTopology m_PrimitiveTopology{};

};

#endif // WATERPIPELINE_H