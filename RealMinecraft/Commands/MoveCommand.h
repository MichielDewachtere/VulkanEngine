#ifndef MOVECOMMAND_H
#define MOVECOMMAND_H

#include <glm/vec3.hpp>

#include <real_core/Command.h>

class Player;

class MoveCommand final : public real::GameObjectCommand
{
public:
	explicit MoveCommand(int id, int controllerId, real::GameObject* pOwner, glm::ivec3 direction);
	virtual ~MoveCommand() override = default;

	MoveCommand(const MoveCommand&) = delete;
	MoveCommand operator=(const MoveCommand&) = delete;
	MoveCommand(MoveCommand&&) = delete;
	MoveCommand operator=(MoveCommand&&) = delete;

	virtual void Execute() override;

private:
	glm::ivec3 m_Direction{ 0,0, 0 };
	float m_Speed{ 5 };

	Player* m_pPlayerComponent{ nullptr };
};

#endif // MOVECOMMAND_H