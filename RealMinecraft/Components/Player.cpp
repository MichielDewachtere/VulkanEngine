#include "Player.h"

#include "Util/Macros.h"

Player::Player(real::GameObject* pOwner)
	: Component(pOwner)
{
}

void Player::UpdateChunkPos(const glm::ivec3& pos)
{
	glm::ivec2 chunkPos;
	chunkPos.x = ((pos.x < 1) ? pos.x - CHUNK_SIZE : pos.x) / CHUNK_SIZE;
	chunkPos.y = ((pos.z < 1) ? pos.z - CHUNK_SIZE : pos.z) / CHUNK_SIZE;

	if (chunkPos != m_CurrentChunk)
	{
		m_CurrentChunk = chunkPos;
		playerPosChanged.Notify(m_CurrentChunk);
	}
}
