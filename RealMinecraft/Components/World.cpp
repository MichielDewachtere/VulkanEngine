#include "World.h"

#include <ranges>
#include <real_core/GameObject.h>

#include "Player.h"
#include "Util/Macros.h"
#include "Components/Chunk.h"
#include "real_core/GameTime.h"
#include "real_core/SceneManager.h"

World::World(real::GameObject* pOwner)
	: Component(pOwner)
{
}

void World::Start()
{
#ifndef SINGLE_CHUNK
	for (int x = -render_distance; x < render_distance + 1; ++x)
	{
		for (int z = -render_distance; z < render_distance + 1; ++z)
		{
			const auto chunkPos = glm::vec3(x * CHUNK_SIZE, 0, z * CHUNK_SIZE);
			auto& go = GetOwner()->CreateGameObject({ chunkPos });

			auto iChunkPos = glm::ivec2{ chunkPos.x, chunkPos.z };
			if (m_BlocksForFutureChunks.contains(iChunkPos))
				m_pChunks[iChunkPos] = go.AddComponent<Chunk>(m_BlocksForFutureChunks.at(iChunkPos));
			else
				m_pChunks[iChunkPos] = go.AddComponent<Chunk>();
		}
	}
#else
	const auto chunkPos = glm::vec3(0, 0, 0);
	auto& go = GetOwner()->CreateGameObject({ chunkPos });
	m_pChunks[glm::vec2{ chunkPos.x, chunkPos.z }] = go.AddComponent<Chunk>();
#endif // SINGLE_CHUNK
	real::SceneManager::GetInstance().GetActiveScene().GetGameObject(1)->GetComponent<Player>()->playerMovedChunk.AddObserver(this);
	real::SceneManager::GetInstance().GetActiveScene().GetGameObject(1)->GetComponent<Player>()->playerMovedBlock.AddObserver(this);

	m_pChunks.at(m_CurrentChunkPos)->SetAsCenter(true);
}

void World::Update()
{
	if (m_ChunksToAdd.empty())
		return;

	const auto [chunkPos, adjacentChunk, direction] = m_ChunksToAdd.front();
	m_ChunksToAdd.pop_front();

	const auto worldPos = glm::vec3{ chunkPos.x,0,chunkPos.y };

	auto& go = GetOwner()->CreateGameObject({ worldPos });

	if (m_BlocksForFutureChunks.contains(chunkPos))
		m_pChunks[chunkPos] = go.AddComponent<Chunk>(m_BlocksForFutureChunks.at(chunkPos));
	else
		m_pChunks[chunkPos] = go.AddComponent<Chunk>();

	m_pChunks.at(adjacentChunk)->UpdateChunkBoarder(m_pChunks.at(chunkPos), direction);

	// Also update the previously added chunk
	const auto otherDir = glm::ivec2{ direction.y * -1, direction.x * -1 };
	const auto otherChunkPos = chunkPos - otherDir;
	if (m_pChunks.contains(otherChunkPos))
		m_pChunks.at(otherChunkPos)->UpdateChunkBoarder(m_pChunks.at(chunkPos), otherDir);

	m_IsDirty = true;
}

void World::LateUpdate()
{
	if (m_FirstFrame)
	{
		m_FirstFrame = false;
		return;
	}

	if (m_IsDirty == false)
		return;

	SortChunks(m_CurrentChunkPos);

	m_IsDirty = false;
}

void World::Kill()
{
	real::SceneManager::GetInstance().GetActiveScene().GetGameObject(1)->GetComponent<Player>()->playerMovedChunk.RemoveObserver(this);
	real::SceneManager::GetInstance().GetActiveScene().GetGameObject(1)->GetComponent<Player>()->playerMovedBlock.RemoveObserver(this);
}

void World::HandleEvent(Player::Events, const glm::ivec2& chunkPos)
{
#ifdef SINGLE_CHUNK
	return;
#endif // SINGLE_CHUNK

	m_pChunks.at(m_CurrentChunkPos)->SetAsCenter(false);
	m_CurrentChunkPos = chunkPos * CHUNK_SIZE;
	m_pChunks.at(m_CurrentChunkPos)->SetAsCenter(true);
	m_IsDirty = true;

	const auto maxX = (chunkPos.x + render_distance) * CHUNK_SIZE;
	const auto minX = (chunkPos.x - render_distance) * CHUNK_SIZE;
	const auto maxY = (chunkPos.y + render_distance) * CHUNK_SIZE;
	const auto minY = (chunkPos.y - render_distance) * CHUNK_SIZE;
	constexpr int chunkShift = (render_distance * 2 + 1) * CHUNK_SIZE;

	constexpr int add = 0, remove = 1, update = 2;
	//std::vector<std::pair<glm::ivec2, int>> dirtyChunks;
	std::map<int, std::vector<glm::ivec2>> dirtyChunks;

	// Register chunks to add/delete/update
	for (const auto& pos : m_pChunks | std::views::keys)
	{
		glm::ivec2 newPos = pos;
		glm::ivec2 chunkToUpdate = pos;
		bool toAdd = false;

		if (pos.x > maxX || pos.x < minX)
		{
			newPos.x += (pos.x > maxX ? -chunkShift : chunkShift);
			chunkToUpdate.x = newPos.x + (pos.x < maxX ? -CHUNK_SIZE : CHUNK_SIZE);

			toAdd = true;
			dirtyChunks[remove].push_back(pos);
			//dirtyChunks.push_back({ pos,remove });
		}

		if (pos.y > maxY || pos.y < minY)
		{
			newPos.y += (pos.y > maxY ? -chunkShift : chunkShift);
			chunkToUpdate.y = newPos.y + (pos.y < maxY ? -CHUNK_SIZE : CHUNK_SIZE);

			toAdd = true;

			if (std::ranges::find(dirtyChunks[remove], pos) == dirtyChunks[remove].end())
				dirtyChunks[remove].push_back(pos);
		}

		if (toAdd)
		{
			dirtyChunks[add].push_back(newPos);

			if (chunkToUpdate.x == pos.x || chunkToUpdate.y == pos.y)
				dirtyChunks[update].push_back(chunkToUpdate);
		}
	}

	glm::ivec2 direction;
	if (dirtyChunks[add].empty())
		return;

	if (dirtyChunks[add].front().x < dirtyChunks[update].front().x)
		direction = { -CHUNK_SIZE,0 };
	else if (dirtyChunks[add].front().x > dirtyChunks[update].front().x)
		direction = { CHUNK_SIZE,0 };
	else if (dirtyChunks[add].front().y < dirtyChunks[update].front().y)
		direction = { 0,-CHUNK_SIZE };
	else if (dirtyChunks[add].front().y > dirtyChunks[update].front().y)
		direction = { 0,CHUNK_SIZE };

	for (const auto& [state, chunks] : dirtyChunks)
	{
		switch (state)
		{
		case add:
		{
			//std::ranges::for_each(chunks, [this](const glm::ivec2& pos)
			//	{
			//		m_ChunksToAdd[pos] = {};
			//	});
			break;
		}
		case remove:
		{
			std::ranges::for_each(chunks, [this](const glm::ivec2& pos)
				{
					m_pChunks[pos]->GetOwner()->Destroy();
					m_pChunks.erase(glm::vec2{ pos.x, pos.y });
				});
			break;
		}
		case update:
		{
			std::ranges::for_each(chunks, [this, direction](const glm::ivec2& pos)
				{
					const auto adjacentChunk = pos + direction;
					m_ChunksToAdd.emplace_back(adjacentChunk, pos, direction);
				});
			break;
		}
		}
	}
}

void World::HandleEvent(Player::Events, const glm::ivec3& playerPos)
{
	m_pChunks.at(m_CurrentChunkPos)->SortBlocks(playerPos);
}

Chunk* World::GetChunkAt(const glm::ivec2& chunkPos) const
{
	//const auto it = std::ranges::find_if(m_pChunks, [chunkPos](const std::pair<glm::ivec2, Chunk*>& pair)
	//	{
	//		return pair.first == chunkPos;
	//	});

	//if (it != m_pChunks.end())
	//{
	//	return it->second;
	//}

	//return nullptr;

	if (m_pChunks.contains(chunkPos))
		return m_pChunks.at(chunkPos);

	return nullptr;
}

void World::AddBlocksForFutureChunks(const glm::ivec2& chunkPos, const std::vector<std::pair<glm::ivec3, EBlock>>&
                                     blocks)
{
	m_BlocksForFutureChunks[chunkPos].insert(m_BlocksForFutureChunks[chunkPos].end(), blocks.begin(), blocks.end());
}

void World::SortChunks(const glm::ivec2& center)
{
	const auto pChild = GetOwner()->GetScene().GetGameObject(1);
	const auto pos = pChild->GetTransform()->GetWorldPosition();
	const glm::vec2 vec2 = { pos.x,pos.z };

	std::vector<std::pair<glm::ivec2, Chunk*>> vec(m_pChunks.begin(), m_pChunks.end());

	std::ranges::sort(vec, [&center](const auto& a, const auto& b)
		{
			auto distanceSquared = [](const glm::ivec2& p1, const glm::ivec2& p2)
				{
					const glm::ivec2 diff = p1 - p2;
					return diff.x * diff.x + diff.y * diff.y;
				};

			return distanceSquared(a.first, center) > distanceSquared(b.first, center);
		});

	for (const auto& chunk : vec | std::views::values)
	{
		GetOwner()->MoveChildBack(chunk->GetOwner());
	}
}
