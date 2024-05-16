#include "Chunk.h"

#include <real_core/GameObject.h>
#include <real_core/GameTime.h>

#include <Material/MaterialManager.h>
#include <Material/Pipelines/PosTexNormPipeline.h>

#include "Util/BlockParser.h"
#include "Util/Enumerations.h"
#include "Util/NoiseManager.h"
#include "Components/World.h"


Chunk::Chunk(real::GameObject* pOwner)
	: Component(pOwner)
{
	m_pWorldComponent = GetOwner()->GetParent()->GetComponent<World>();

	const auto id = real::GameTime::GetInstance().StartTimer();
	const auto worldPos = GetOwner()->GetTransform()->GetWorldPosition();

	for (size_t x = 0; x < m_Blocks.size(); ++x)
	{
		for (size_t z = 0; z < m_Blocks[x].size(); ++z)
		{
			const auto adjustedX = INT_MAX / 2 + static_cast<double>(worldPos.x) + static_cast<double>(x);
			const auto adjustedZ = INT_MAX / 2 + static_cast<double>(worldPos.z) + static_cast<double>(z);

			float terrainNoiseValue = NoiseManager::GetInstance().GetTerrainNoiseValue(adjustedX, adjustedZ, CHUNK_SIZE);

			terrainNoiseValue *= 10.f;
			//terrainNoiseValue *= 30.f;
			terrainNoiseValue += 60.f;

			const int yLevel = static_cast<int>(terrainNoiseValue);

			m_Blocks[x][z].fill(EBlock::air);
			m_Blocks[x][z][yLevel] = EBlock::grassBlock;
			std::fill_n(m_Blocks[x][z].begin() + yLevel - 3, 3, EBlock::dirt);
			std::fill_n(m_Blocks[x][z].begin(), yLevel - 3, EBlock::stone);

#ifdef SINGLE_CHUNK
			if (x == 0 || x == CHUNK_SIZE - 1
				|| z == 0 || z == CHUNK_SIZE - 1)
			{
				for (size_t y = 0; y <= static_cast<size_t>(yLevel); ++y)
				{
					m_RenderedBlocks[glm::vec3{ x,y,z }] = { true, {} };
				}
			}
#endif // SINGLE_CHUNK

			m_LowestY = std::min(yLevel, m_LowestY);
			m_HighestY = std::max(yLevel, m_HighestY);
		}
	}

	int minYLevel = m_LowestY;
	int maxYLevel = m_HighestY;

	glm::ivec2 dirs[] = { glm::ivec2{-CHUNK_SIZE,0},glm::ivec2{CHUNK_SIZE,0} ,glm::ivec2{0,-CHUNK_SIZE} ,glm::ivec2{0,CHUNK_SIZE} };
	const auto chunkPos = glm::ivec2{ worldPos.x, worldPos.z };
	for (const auto& dir : dirs)
	{
		const Chunk* pOtherChunk = m_pWorldComponent->GetChunkAt(chunkPos + dir);
		if (pOtherChunk == nullptr)
			continue;

		minYLevel = std::min(minYLevel, pOtherChunk->m_LowestY);
		maxYLevel = std::max(maxYLevel, pOtherChunk->m_HighestY);
	}

	// TODO: This could be done more efficiently
	for (size_t x = 0; x < m_Blocks.size(); ++x) 
	{
		const auto& blockLayer = m_Blocks[x];
		for (size_t z = 0; z < m_Blocks[x].size(); ++z)
		{
			const auto& blockRow = blockLayer[z];
			for (size_t y = minYLevel; y <= std::min(static_cast<size_t>(maxYLevel), blockRow.size() - 1); ++y) 
			{
				if (blockRow[y] == EBlock::air)
					continue;
	
				m_RenderedBlocks[glm::vec3{ x, y, z }] = { true, {} };
			}
		}
	}

	auto time = real::GameTime::GetInstance().EndTimer(id);
	std::cout << "time to initialize chunk " << time << " milliseconds\n";
}

void Chunk::Start()
{
	const auto context = real::RealEngine::GetGameContext();

	const auto id = real::GameTime::GetInstance().StartTimer();
	auto [vertices, indices] = CalculateMeshData();
	auto time = real::GameTime::GetInstance().EndTimer(id);
	std::cout << "time to calculate initial mesh data " << time << " milliseconds\n";

	real::MeshInfo info;
	info.vertexCapacity = static_cast<uint32_t>(vertices.size());
	info.indexCapacity = static_cast<uint32_t>(indices.size());
	info.usesUbo = true;
	info.texture = real::ContentManager::GetInstance().LoadTexture(context, "Resources/textures/atlas.png");

	m_pMeshComponent = GetOwner()->AddComponent<real::MeshIndexed<real::PosTexNorm>>(info);
	real::MaterialManager::GetInstance().GetMaterial<real::PosTexNormPipeline, real::PosTexNorm>()->BindMesh(context, m_pMeshComponent);

	m_pMeshComponent->SetVertices(vertices);
	m_pMeshComponent->SetIndices(indices);
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
			m_Blocks[10][8][m_AddBlock] = EBlock::oakLog;

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

	if (m_IsDirty == false)
		return;

	auto id = real::GameTime::GetInstance().StartTimer();
	auto [vertices, indices] = CalculateMeshData();
	auto time = real::GameTime::GetInstance().EndTimer(id);
	std::cout << "time to calculate mesh data " << time << " milliseconds\n";

	m_pMeshComponent->SetIndices(indices);
	m_pMeshComponent->SetVertices(vertices);

	m_IsDirty = false;
}

void Chunk::UpdateChunkBoarder(const Chunk* adjacentChunk, const glm::ivec2& dir)
{
	int otherX = -1, otherZ = -1;
	int thisX = -1, thisZ = -1;

	if (dir.x == -CHUNK_SIZE)
	{
		otherX = CHUNK_SIZE - 1;
		thisX = 0;
	}
	else if (dir.x == CHUNK_SIZE)
	{
		otherX = 0;
		thisX = CHUNK_SIZE - 1;
	}
	else if (dir.y == -CHUNK_SIZE)
	{
		otherZ = CHUNK_SIZE - 1;
		thisZ = 0;
	}
	else if (dir.y == CHUNK_SIZE)
	{
		otherZ = 0;
		thisZ = CHUNK_SIZE - 1;
	}

	const auto lowestY = std::min(m_LowestY, adjacentChunk->m_LowestY);
	const auto highestY = std::max(m_HighestY, adjacentChunk->m_HighestY);

	auto updateBlocks = [&](int size1, int fixedIndex1, int fixedIndex2, bool isXFixed) {
		for (int i = 0; i < size1; ++i)
		{
			for (int y = lowestY; y < highestY; ++y)
			{
				const int x = isXFixed ? fixedIndex1 : i;
				const int z = isXFixed ? i : fixedIndex2;

				if (adjacentChunk->m_Blocks[x][z][y] == EBlock::air && m_Blocks[x][z][y] != EBlock::air)
				{
					m_RenderedBlocks[glm::vec3{ x, y, z }] = { true, {} };
				}
			}
		}
		};

	if (otherX != -1)
	{
		updateBlocks(static_cast<int>(m_Blocks[0].size()), otherX, thisX, true);
	}
	else if (thisZ != -1)
	{
		updateBlocks(static_cast<int>(m_Blocks.size()), otherZ, thisZ, false);
	}

	m_IsDirty = true;
}

//TODO: Improve this
std::pair<std::vector<real::PosTexNorm>, std::vector<uint32_t>> Chunk::CalculateMeshData()
{
	std::vector<real::PosTexNorm> vertices;
	std::vector<uint32_t> indices;

	constexpr glm::vec3 dirs[6] = { {0,0,-1},{1,0,0},{0,0,1},{-1,0,0},{0,1,0},{0,-1,0} };
	//const auto chunkPos = GetOwner()->GetTransform()->GetWorldPosition();

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

				auto accuPos = /*chunkPos + */pos;

				if (CanRenderFace(static_cast<size_t>(pos.x + dirOffset.x), static_cast<size_t>(pos.z + dirOffset.z), static_cast<size_t>(pos.y + dirOffset.y)) == false)
					continue;

				++counter;

				auto v = BlockParser::GetInstance().GetFaceData(dir, block, accuPos);

				if (v.size() < 4)
					std::cout << "test\n";

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

bool Chunk::CanRenderFace(int x, int z, int y) const
{

#ifdef SINGLE_CHUNK
	// Check if coordinates are in bound
	if ((x < 0 || x >= CHUNK_SIZE)
		 || (y < 0 || y >= CHUNK_HEIGHT)
		 || (z < 0 || z >= CHUNK_SIZE))
	return true;
#else
	if (y < 0 || y >= CHUNK_HEIGHT)
		return true;

	// Check if coordinates are in bound
	if ((x < 0 || x >= CHUNK_SIZE)
		 || (z < 0 || z >= CHUNK_SIZE))
	{
		const auto pos = GetOwner()->GetTransform()->GetWorldPosition();
		auto chunkPos = glm::vec2(pos.x, pos.z);

		if (x < 0)
			chunkPos.x -= CHUNK_SIZE;
		else if (x >= CHUNK_SIZE)
			chunkPos.x += CHUNK_SIZE;

		if (z < 0)
			chunkPos.y -= CHUNK_SIZE;
		else if (z >= CHUNK_SIZE)
			chunkPos.y += CHUNK_SIZE;

		const Chunk* pOtherChunk = m_pWorldComponent->GetChunkAt(chunkPos);
		if (pOtherChunk == nullptr)
			return false;

		const int blockX = (x < 0) ? CHUNK_SIZE - 1 : (x >= CHUNK_SIZE) ? 0 : x;
		const int blockZ = (z < 0) ? CHUNK_SIZE - 1 : (z >= CHUNK_SIZE) ? 0 : z;

		return pOtherChunk->m_Blocks[blockX][blockZ][y] == EBlock::air;
	}
#endif // SINGLE_CHUNK

	return m_Blocks[x][z][y] == EBlock::air;
}
