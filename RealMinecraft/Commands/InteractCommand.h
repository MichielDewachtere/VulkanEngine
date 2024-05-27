#ifndef INTERACTCOMMAND_H
#define INTERACTCOMMAND_H

#include <glm/vec3.hpp>

#include <real_core/Command.h>

class OutlineBlock;
class Player;
class World;

class InteractCommand final : public real::GameObjectCommand
{
public:
	explicit InteractCommand(int id, int controllerId, real::GameObject* pOwner, World* pWorldComponent, bool place);
	virtual ~InteractCommand() override = default;

	InteractCommand(const InteractCommand&) = delete;
	InteractCommand operator=(const InteractCommand&) = delete;
	InteractCommand(InteractCommand&&) = delete;
	InteractCommand operator=(InteractCommand&&) = delete;

	virtual void Execute() override;

private:
	bool m_Place;
	World* m_pWorldComponent;

	OutlineBlock* m_pOutlineBlockComponent{ nullptr };
	//Player* m_pPlayerComponent{ nullptr };
};

#endif // INTERACTCOMMAND_H