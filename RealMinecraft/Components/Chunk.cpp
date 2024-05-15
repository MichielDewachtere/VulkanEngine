#include "Chunk.h"

#include <real_core/GameObject.h>

#include "Material/MaterialManager.h"
#include "Material/Pipelines/PosTexNormPipeline.h"
#include "real_core/GameTime.h"
#include "Util/BlockParser.h"
#include "Util/Enumerations.h"


Chunk::Chunk(real::GameObject* pOwner)
	: Component(pOwner)
{
	const auto id = real::GameTime::GetInstance().StartTimer();

	for (size_t x = 0; x < m_Blocks.size(); ++x)
	{
		for (size_t z = 0; z < m_Blocks[x].size(); ++z)
		{
			m_Blocks[x][z].fill(EBlock::air);
			std::fill_n(m_Blocks[x][z].begin(), 64, EBlock::debug);

			if (x == 0 || x == CHUNK_SIZE - 1
				|| z == 0 || z == CHUNK_SIZE - 1)
			{
				for (size_t y = 0; y < 64; ++y)
				{
					m_RenderedBlocks[glm::vec3{ x,y,z }] = { true, {} };
				}
			}
			else
			{
				m_RenderedBlocks[glm::vec3{ x,63,z }] = { true, {} };
				m_RenderedBlocks[glm::vec3{ x,0,z }] = { true, {} };
			}

			//for (size_t y = 0; y < m_Blocks[x][z].size(); ++y)
			//{
			//	if ()
			//	m_Blocks[x][z][y] = EBlock::stone;
			//}
		}
	}

	auto time = real::GameTime::GetInstance().EndTimer(id);
	std::cout << "time to initialize chunk " << time << " milliseconds\n";
}

void Chunk::Start()
{
	const auto context = real::RealEngine::GetGameContext();

	auto [vertices, indices] = CalculateMeshData();

	real::MeshInfo info;
	info.vertexCapacity = static_cast<uint32_t>(vertices.size());
	//info.vertexCapacity = static_cast<uint32_t>(2624);
	info.indexCapacity = static_cast<uint32_t>(indices.size());
	//info.indexCapacity = static_cast<uint32_t>(3936);
	info.usesUbo = true;
	info.texture = real::ContentManager::GetInstance().LoadTexture(context, "Resources/textures/atlas.png");

	m_pMeshComponent = GetOwner()->AddComponent<real::MeshIndexed<real::PosTexNorm>>(info);
	real::MaterialManager::GetInstance().GetMaterial<real::PosTexNormPipeline, real::PosTexNorm>()->BindMesh(context, m_pMeshComponent);

	m_pMeshComponent->AddVertices(vertices);
	m_pMeshComponent->AddIndices(indices);
	m_pMeshComponent->Init(real::RealEngine::GetGameContext());
}

void Chunk::Update()
{
	//m_AccuTime += real::GameTime::GetInstance().GetElapsed();

	if (m_AccuTime >= m_BlockRemoveTime)
	{
		// Remove Blocks
		{
			m_Blocks[8][8][m_RemoveBlock] = EBlock::air;

			// Add surrounding blocks to m_RenderedBlocks
			m_RenderedBlocks[glm::vec3{ 7,m_RemoveBlock,8 }] = { true,{} };
			m_RenderedBlocks[glm::vec3{ 9,m_RemoveBlock,8 }] = { true,{} };
			m_RenderedBlocks[glm::vec3{ 8,m_RemoveBlock,7 }] = { true,{} };
			m_RenderedBlocks[glm::vec3{ 8,m_RemoveBlock,9 }] = { true,{} };
			m_RenderedBlocks[glm::vec3{ 8,m_RemoveBlock - 1,8 }] = { true,{} };

			// Remove block from m_RenderedBlocks
			m_RenderedBlocks.erase(glm::vec3{ 8,m_RemoveBlock,8 });
		}
		// Add Blocks
		{
			m_Blocks[10][8][m_AddBlock] = EBlock::debug;

			m_RenderedBlocks[glm::vec3{ 10,m_AddBlock,8 }] = { true,{} };
			m_RenderedBlocks.at(glm::vec3{ 10,m_AddBlock - 1,8 }).first = true;
		}

		auto id = real::GameTime::GetInstance().StartTimer();
		auto [vertices, indices] = CalculateMeshData();
		auto time = real::GameTime::GetInstance().EndTimer(id);
		std::cout << "time to calculate mesh data " << time << " milliseconds\n";

		m_pMeshComponent->SetIndices(indices);
		m_pMeshComponent->SetVertices(vertices);
		//m_pMeshComponent->Init(real::RealEngine::GetGameContext());

		--m_RemoveBlock;
		++m_AddBlock;
		m_AccuTime = 0;
	}

	//if (m_IsDirty == false)
	//	return;

	//m_IsDirty = false;
}

//TODO: Improve this
std::pair<std::vector<real::PosTexNorm>, std::vector<uint32_t>> Chunk::CalculateMeshData()
{
	std::vector<real::PosTexNorm> vertices;
	std::vector<uint32_t> indices;

	constexpr glm::vec3 dirs[6] = { {0,0,-1},{1,0,0},{0,0,1},{-1,0,0},{0,1,0},{0,-1,0} };
	const auto chunkPos = GetOwner()->GetTransform()->GetWorldPosition();

	std::vector<glm::vec3> blocksToRemove;

	for (auto& [pos, data] : m_RenderedBlocks)
	{
		if (data.first)
		{
			data.second.clear();

			int counter = 0;
			for (int i = 0; i < static_cast<int>(EDirection::amountOfDirections); ++i)
			{
				const auto dir = static_cast<EDirection>(i);
				const auto block = m_Blocks[static_cast<size_t>(pos.x)][static_cast<size_t>(pos.z)][static_cast<size_t>(pos.y)];
				const auto dirOffset = dirs[i];

				auto accuPos = chunkPos + pos;

				if (CanRenderFace(static_cast<size_t>(pos.x + dirOffset.x), static_cast<size_t>(pos.z + dirOffset.z), static_cast<size_t>(pos.y + dirOffset.y)) == false)
					continue;

				++counter;

				auto v = BlockParser::GetInstance().GetFaceData(dir, block, accuPos);
				data.second.insert(data.second.end(), v.begin(), v.end());

				const auto offset = static_cast<uint32_t>(vertices.size());
				vertices.insert(vertices.end(), v.begin(), v.end());
				indices.insert(indices.end(), { 0 + offset, 1 + offset, 2 + offset, 2 + offset, 3 + offset, 0 + offset });
			}

			if (counter == 0)
				blocksToRemove.push_back(pos);

			data.first = false;
		}
		else
		{
			for (int i = 0; i < data.second.size() / 4; ++i)
			{
				const auto offset = static_cast<uint32_t>(vertices.size());
				vertices.insert(vertices.end(), data.second.begin() + i * 4, data.second.begin() + (i + 1) * 4);
				indices.insert(indices.end(), { 0 + offset, 1 + offset, 2 + offset, 2 + offset, 3 + offset, 0 + offset });
			}
		}
	}

	std::ranges::for_each(blocksToRemove, [this](const glm::vec3& pos) { m_RenderedBlocks.erase(pos); });

	return { vertices, indices };
}

bool Chunk::CanRenderFace(size_t x, size_t z, size_t y) const
{
#ifdef SINGLE_CHUNK
	// Check if coordinates are in bound
	if ((x < 0 || x >= CHUNK_SIZE)
		 || (y < 0 || y >= CHUNK_HEIGHT)
		 || (z < 0 || z >= CHUNK_SIZE))
	return true;
#else
	// Check if coordinates are in bound
	if ((x < 0 || x >= CHUNK_SIZE)
		 || (z < 0 || z >= CHUNK_SIZE))
	return false;

	if (y < 0 || y >= CHUNK_HEIGHT)
		return true;
#endif // SINGLE_CHUNK

	return m_Blocks[x][z][y] == EBlock::air;
}
