#include "InteractCommand.h"

#include "real_core/GameObject.h"
#include "real_core/Utils.h"
#include "Util/Macros.h"

#include <cmath>

#include "Components/World.h"
#include "Components/Chunk.h"
#include "Components/OutlineBlock.h"
#include "Util/Enumerations.h"

InteractCommand::InteractCommand(int id, int controllerId, real::GameObject* pOwner, World* pWorldComponent, bool place)
    : GameObjectCommand(id, controllerId, pOwner)
    , m_Place(place)
    , m_pWorldComponent(pWorldComponent)
{
        m_pOutlineBlockComponent = GetGameObject()->GetComponent<OutlineBlock>();
}

void InteractCommand::Execute()
{
    if (m_pOutlineBlockComponent->HasBlockSelected() == false)
        return;

    if (m_Place == false)
    {
        auto [chunkPos, blockPos] = m_pOutlineBlockComponent->GetSelectedBlock();
        m_pWorldComponent->GetChunkAt(chunkPos)->SetBlock(blockPos, EBlock::air);
    }
    else
    {
        auto [chunkPos, blockPos] = m_pOutlineBlockComponent->GetPosToPlace();
    	m_pWorldComponent->GetChunkAt(chunkPos)->SetBlock(blockPos, EBlock::stone);
    }
}
