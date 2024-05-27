#ifndef PLAYER_H
#define PLAYER_H

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <real_core/Component.h>
#include <real_core/Subject.h>

class Player final : public real::Component
{
public:
	enum class Events
	{
		movedChunk = 0,
		movedBlock = 1
	};

	explicit Player(real::GameObject* pOwner);
	~Player() override = default;

	Player(const Player& other) = delete;
	Player& operator=(const Player& rhs) = delete;
	Player(Player&& other) = delete;
	Player& operator=(Player&& rhs) = delete;

	void UpdateChunkPos(const glm::vec3& pos);

	real::Subject<Events, const glm::ivec2&> playerMovedChunk;
	real::Subject<Events, const glm::ivec3&> playerMovedBlock;

private:
	glm::ivec2 m_CurrentChunk{ 0,0 };
	glm::ivec3 m_CurrentBlock{ 0,0,0 };
};

#endif // PLAYER_H