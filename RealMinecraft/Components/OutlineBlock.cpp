#include "OutlineBlock.h"

#include "real_core/GameObject.h"
#include "Util/Enumerations.h"
#include "Util/Macros.h"
#include "Components/World.h"
#include "Components/Chunk.h"

OutlineBlock::OutlineBlock(real::GameObject* pOwner, real::GameObject* player, World* world, int reach)
    : Component(pOwner)
    , m_pPlayerTransform(player->GetTransform())
    , m_pWorldComponent(world)
    , m_Reach(reach)
{
}

void OutlineBlock::Start()
{
	m_pMeshComponent = GetOwner()->GetComponent<real::MeshIndexed<real::PosColNorm, real::UniformBufferObject>>();
}

void OutlineBlock::Update()
{
    const auto worldPos = m_pPlayerTransform->GetWorldPosition();
    const auto forward = m_pPlayerTransform->GetForward();

	for (float i = 1; i < static_cast<float>(m_Reach); i += 0.1f)  // NOLINT(cert-flp30-c)
    {
        const auto fTargetPos = worldPos - forward * i;
        glm::ivec3 targetPos;
        targetPos.x = static_cast<int>(floor(fTargetPos.x));
        targetPos.y = static_cast<int>(floor(fTargetPos.y));
        targetPos.z = static_cast<int>(ceil(fTargetPos.z));

        glm::ivec2 chunkPos;
        chunkPos.x = targetPos.x < 0 ? ((targetPos.x + 1) - CHUNK_SIZE) / CHUNK_SIZE * CHUNK_SIZE : targetPos.x / CHUNK_SIZE * CHUNK_SIZE;
        chunkPos.y = targetPos.z < 0 ? ((targetPos.z + 1) - CHUNK_SIZE) / CHUNK_SIZE * CHUNK_SIZE : targetPos.z / CHUNK_SIZE * CHUNK_SIZE;

        glm::ivec3 blockPos;

        blockPos.x = targetPos.x % CHUNK_SIZE;
        blockPos.y = targetPos.y;
        blockPos.z = targetPos.z % CHUNK_SIZE;

        if (targetPos.x == -16 || targetPos.z == -16)
            std::cout << "test\n";

        if (targetPos.x < 0 && blockPos.x != 0) blockPos.x = CHUNK_SIZE + blockPos.x;
        if (targetPos.z < 0 && blockPos.z != 0) blockPos.z = CHUNK_SIZE + blockPos.z;

        const auto pChunk = m_pWorldComponent->GetChunkAt(chunkPos);
        if (pChunk == nullptr)
            continue;

        if (pChunk->IsBlockAir(blockPos) || pChunk->IsBlockWater(blockPos))
        {
            m_CanPlaceAt = { chunkPos, blockPos };
        }
        else
        {
            GetOwner()->GetTransform()->SetWorldPosition(glm::vec3(targetPos));

            m_SelectedBlock = { chunkPos, blockPos };
            m_HasBlockSelected = true;

            m_pMeshComponent->Enable();
            return;
        }
    }

    m_HasBlockSelected = false;
    m_CanPlaceAt = {};
    m_SelectedBlock = {};
    m_pMeshComponent->Disable();
}
