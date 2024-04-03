#ifndef	DEPTHBUFFERMANAGER_H
#define DEPTHBUFFERMANAGER_H

#include <cstdint>
#include <memory>

#include "Util/Singleton.h"
#include "DepthBuffer.h"

class DepthBufferManager final : public Singleton<DepthBufferManager>
{
public:
	virtual ~DepthBufferManager() override = default;

	void CleanUp(const GameContext& context);

	DepthBuffer* GetDepthBuffer(uint32_t id) const;
	uint32_t AddDepthBuffer(const GameContext& context);

private:
	friend class Singleton<DepthBufferManager>;
	DepthBufferManager() = default;

	std::vector<std::unique_ptr<DepthBuffer>> m_pDepthBuffers{};
};
#endif // DEPTHBUFFERMANAGER_H