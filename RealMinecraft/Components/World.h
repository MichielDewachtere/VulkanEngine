#ifndef	WORLD_H
#define WORLD_H

#include <deque>
#include <map>
#include <vector>

#include <glm/vec2.hpp>

#include <real_core/Component.h>
#include <real_core/Observer.h>

#include "Player.h"

enum class EBlock;
class Chunk;

template<glm::length_t L, typename T, glm::qualifier Q = glm::defaultp>
struct VecComparator
{
	bool operator()(const glm::vec<L, T, Q>& lhs, const glm::vec<L, T, Q>& rhs) const
	{
		for (int i = 0; i < L; ++i) 
		{
			if (lhs[i] < rhs[i]) return true;
			if (lhs[i] > rhs[i]) return false;
		}
		return false;
	}
};

class World final
	: public real::Component
	, public real::Observer<Player::Events, const glm::ivec2&>
	, public real::Observer<Player::Events, const glm::ivec3&>
{
public:
	explicit World(real::GameObject* pOwner);
	virtual ~World() override = default;

	World(const World& other) = delete;
	World& operator=(const World& rhs) = delete;
	World(World&& other) = delete;
	World& operator=(World&& rhs) = delete;

	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void Kill() override;

	void HandleEvent(Player::Events, const glm::ivec2&) override;
	void HandleEvent(Player::Events, const glm::ivec3&) override;
	void OnSubjectDestroy() override {}

	Chunk* GetChunkAt(const glm::ivec2& chunkPos) const;
	void AddBlocksForFutureChunks(const glm::ivec2& chunkPos, const std::vector<std::pair<glm::ivec3, EBlock>>& blocks);

private:
	static constexpr inline int render_distance{ 12 };
	bool m_IsDirty{ true }, m_FirstFrame{ true };
	glm::ivec2 m_CurrentChunkPos{ 0,0 };

	std::map<glm::ivec2, Chunk*, VecComparator<2, int>> m_pChunks{};
	std::map<glm::ivec2, std::vector<std::pair<glm::ivec3, EBlock>>, VecComparator<2, int>> m_BlocksForFutureChunks{};

	std::deque<std::tuple<glm::ivec2, glm::ivec2, glm::ivec2>> m_ChunksToAdd{};

	void SortChunks(const glm::ivec2& center);
};

#endif // WORLD_H