#include "DepthBufferManager.h"

#include <algorithm>

void DepthBufferManager::CleanUp(const GameContext& context)
{
	std::ranges::for_each(m_pDepthBuffers, [context](const auto& b)
		{
			b->CleanUp(context);
		});
}

DepthBuffer* DepthBufferManager::GetDepthBuffer(uint32_t id) const
{
	return m_pDepthBuffers.at(id).get();
}

uint32_t DepthBufferManager::AddDepthBuffer(const GameContext& context)
{
	m_pDepthBuffers.push_back(std::make_unique<DepthBuffer>(context));
	return static_cast<uint32_t>(m_pDepthBuffers.size());
}
