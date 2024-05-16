#include "World.h"

#include <ranges>
#include <real_core/GameObject.h>

#include "Player.h"
#include "Util/Macros.h"
#include "Components/Chunk.h"
#include "real_core/GameTime.h"

World::World(real::GameObject* pOwner)
	: Component(pOwner)
{
}

void World::Start()
{
	for (int x = -render_distance; x < render_distance + 1; ++x)
	{
		for (int z = -render_distance; z < render_distance + 1; ++z)
		{
			const auto chunkPos = glm::vec3(x * CHUNK_SIZE, 0, z * CHUNK_SIZE);
			auto& go = GetOwner()->CreateGameObject({ chunkPos });
			m_pChunks[glm::vec2{ chunkPos.x, chunkPos.z }] = go.AddComponent<Chunk>();
		}
	}

	real::SceneManager::GetInstance().GetActiveScene().GetGameObject(1)->GetComponent<Player>()->playerPosChanged.AddObserver(this);
}

void World::Update()
{
}

void World::Kill()
{
	real::SceneManager::GetInstance().GetActiveScene().GetGameObject(1)->GetComponent<Player>()->playerPosChanged.RemoveObserver(this);
}

void World::HandleEvent(const glm::ivec2& chunkPos)
{
	const auto id = real::GameTime::GetInstance().StartTimer();
	std::cout << "In chunk: [" << chunkPos.x << ';' << chunkPos.y << "]\n";

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
			std::ranges::for_each(chunks, [this](const glm::ivec2& pos)
				{
					auto worldPos = glm::vec3{ pos.x,0,pos.y };
					std::cout << "added chunk at position " << worldPos.x << ',' << worldPos.y << ',' << worldPos.z << '\n';

					auto& go = GetOwner()->CreateGameObject({ worldPos });
					m_pChunks[glm::vec2{ worldPos.x, worldPos.z }] = go.AddComponent<Chunk>();
				});
			break;
		}
		case remove:
		{
			std::ranges::for_each(chunks, [this](const glm::ivec2& pos)
				{
					std::cout << "Removing chunk at position " << pos.x << ',' << 0 << ',' << pos.y << '\n';

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
					std::cout << "Updating chunk at position " << pos.x << ',' << 0 << ',' << pos.y << '\n';
					std::cout << "Adjacent chunk has position " << adjacentChunk.x << ',' << 0 << ',' << adjacentChunk.y << '\n';
					m_pChunks[pos]->UpdateChunkBoarder(m_pChunks[adjacentChunk], direction);
				});
			break;
		}
		}
	}

	auto time = real::GameTime::GetInstance().EndTimer(id);
	std::cout << "time to update world " << time << " milliseconds\n";
}

Chunk* World::GetChunkAt(const glm::ivec2& chunkPos) const
{
	if (m_pChunks.contains(chunkPos) == false)
		return nullptr;

	return m_pChunks.at(chunkPos);
}
