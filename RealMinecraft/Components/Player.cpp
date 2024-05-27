#include "Player.h"

#include <real_core/Utils.h>

#include "Util/Macros.h"

Player::Player(real::GameObject* pOwner)
	: Component(pOwner)
{
}

void Player::UpdateChunkPos(const glm::vec3& pos)
{
	auto fPos = pos;

	if (const auto z = static_cast<int>(fPos.z) % 16;
		z == 15 || z == 0)
		fPos.z += 1;

	auto iPos = glm::ivec3(fPos);

	if (iPos != m_CurrentBlock)
	{
		m_CurrentBlock = iPos;
		playerMovedBlock.Notify(Events::movedBlock, { m_CurrentBlock.x % CHUNK_SIZE, m_CurrentBlock.y, m_CurrentBlock.z % CHUNK_SIZE });
	}

	glm::ivec2 chunkPos;
	chunkPos.x = ((fPos.x <= 0) ? iPos.x - CHUNK_SIZE : iPos.x) / CHUNK_SIZE;
	chunkPos.y = ((fPos.z < 0) ? iPos.z - CHUNK_SIZE : iPos.z) / CHUNK_SIZE;

	if (chunkPos != m_CurrentChunk)
	{
		m_CurrentChunk = chunkPos;
		playerMovedChunk.Notify(Events::movedChunk, m_CurrentChunk);
	}
}
