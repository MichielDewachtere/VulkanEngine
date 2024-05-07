#include "Command.h"

#include <iostream>

#include "GameObject.h"

real::GameObjectCommand::GameObjectCommand(int id, int controllerId, GameObject* pGameObject)
	: Command(id, controllerId), m_pGameObject(pGameObject)
{
}

void real::TestCommand::Execute()
{
	std::cout << "Test command succeeded\n";
}
