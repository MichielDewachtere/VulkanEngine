#include "Chunk.h"

#include <real_core/GameObject.h>
#include <real_core/GameTime.h>
#include <real_core/Utils.h>

#include <Material/MaterialManager.h>

#include <Misc/AABB.h>

#include "Util/BlockParser.h"
#include "Util/Enumerations.h"
#include "Util/NoiseManager.h"
#include "Components/World.h"

#include "Materials/DiffuseMaterial.h"
#include "Misc/Camera.h"
#include "Misc/CameraManager.h"

Chunk::Chunk(real::GameObject* pOwner, const std::vector<std::pair<glm::ivec3, EBlock>>& blocks)
	: Component(pOwner)
{
	m_pWorldComponent = GetOwner()->GetParent()->GetComponent<World>();

	const auto id = real::GameTime::GetInstance().StartTimer();
	const auto worldPos = GetOwner()->GetTransform()->GetWorldPosition();

	for (size_t x = 0; x < m_Blocks.size(); ++x)
	{
		for (size_t z = 0; z < m_Blocks[x].size(); ++z)
		{

			m_Blocks[x][z].fill(EBlock::air);
		}
	}

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

			if (yLevel <= WATER_LEVEL)
			{
				std::fill_n(m_Blocks[x][z].begin() + yLevel, WATER_LEVEL - yLevel, EBlock::water);
				m_Blocks[x][z][yLevel] = EBlock::sand;
			}
			else
			{
				m_Blocks[x][z][yLevel] = EBlock::grassBlock;
				GenerateTree({ x, yLevel, z });
				GenerateFlower({ x, yLevel, z });

				if (x == 8 && z == 8 && m_Blocks[x][z][yLevel + 1] == EBlock::air)
					m_Blocks[x][z][yLevel + 1] = EBlock::poppy;
			}
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

	if (blocks.empty() == false)
	{
		for (const auto& [pos, type] : blocks)
		{
			m_Blocks[pos.x][pos.z][pos.y] = type;
			m_HighestY = std::max(m_HighestY, pos.y);
		}
	}

	m_HighestY = std::max(m_HighestY, WATER_LEVEL);

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

	const auto activeCamera = real::CameraManager::GetInstance().GetActiveCamera();
	const auto cameraPos = GetOwner()->GetTransform()->GetWorldPosition();
	const auto boxMax = cameraPos + glm::vec3{ CHUNK_SIZE, m_HighestY, CHUNK_SIZE };

	real::AABB aabb;
	aabb.min = worldPos;
	aabb.max = worldPos + glm::vec3{ CHUNK_SIZE, m_HighestY, CHUNK_SIZE };
	m_Aabb = aabb;
}

void Chunk::Start()
{
	const auto context = real::RealEngine::GetGameContext();

	InitSolidChunk(context);
	InitTransparentChunk();
}

void Chunk::Update()
{
	const auto activeCamera = real::CameraManager::GetInstance().GetActiveCamera();
	const auto worldPos= GetOwner()->GetTransform()->GetWorldPosition();

	if (real::FrustumAABB::IsBoxInFrustum(activeCamera->GetViewProjection(), m_Aabb) == false)
	{
		m_pTransparentMeshComponent->Disable();
		m_pSolidMeshComponent->Disable();
	}
	else
	{
		m_pTransparentMeshComponent->Enable();
		m_pSolidMeshComponent->Enable();
	}

	if (m_IsDirty == false)
		return;

	real::AABB aabb;
	aabb.min = worldPos;
	aabb.max = worldPos + glm::vec3{ CHUNK_SIZE, m_HighestY, CHUNK_SIZE };
	m_Aabb = aabb;

	auto [vertices, indices] = CalculateMeshData();
	m_pSolidMeshComponent->SetIndices(indices);
	m_pSolidMeshComponent->SetVertices(vertices);

	const auto faces = CalculateTransparentMeshData();
	m_pTransparentMeshComponent->SetFaces(faces);
	m_pTransparentMeshComponent->SortFaces(activeCamera->GetOwner()->GetTransform()->GetWorldPosition(), m_ChunkIsCenter);

	m_IsDirty = false;
}

void Chunk::UpdateChunkBoarder(const Chunk* adjacentChunk, const glm::ivec2& dir)
{
	auto& blockParser = BlockParser::GetInstance();

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

	auto updateBlocks = [&](int size1, int thisCoord, int otherCoord, bool isXFixed) {
		for (int i = 0; i < size1; ++i)
		{
			for (int y = lowestY; y < highestY; ++y)
			{
				EBlock currentBlock, otherBlock;
				if (isXFixed)
				{
					currentBlock = m_Blocks[i][thisCoord][y];
					otherBlock = adjacentChunk->m_Blocks[i][otherCoord][y];
				}
				else
				{
					currentBlock = m_Blocks[thisCoord][i][y];
					otherBlock = adjacentChunk->m_Blocks[otherCoord][i][y];
				}

				if (currentBlock == EBlock::air)// && otherBlock == EBlock::air)
					continue;

				if (currentBlock != EBlock::air && otherBlock == EBlock::air
					|| !blockParser.IsTransparent(currentBlock) && blockParser.IsTransparent(otherBlock)
					|| blockParser.IsTransparent(currentBlock) && blockParser.IsTransparent(otherBlock) && (otherBlock != currentBlock || otherBlock == EBlock::oakLeaves && currentBlock == EBlock::oakLeaves))
				{
					m_RenderedBlocks[glm::vec3{ isXFixed ? i : thisCoord, y, isXFixed ? thisCoord : i }] = { true, {} };
				}
			}
		}
		};

	if (thisX != -1)
	{
		updateBlocks(static_cast<int>(m_Blocks[0].size()), thisX, otherX, false);
	}
	else if (thisZ != -1)
	{
		updateBlocks(static_cast<int>(m_Blocks.size()), thisZ, otherZ, true);
	}

	m_IsDirty = true;
}

void Chunk::SortBlocks(const glm::ivec3& position) const
{
	m_pTransparentMeshComponent->SortFaces(position);
}

bool Chunk::IsBlockAir(const glm::ivec3& pos) const
{
	if (IsPosValid(pos) == false)
		return true;

	return m_Blocks[pos.x][pos.z][pos.y] == EBlock::air;
}

bool Chunk::IsBlockWater(const glm::ivec3& pos) const
{
	if (IsPosValid(pos) == false)
		return true;

	return m_Blocks[pos.x][pos.z][pos.y] == EBlock::water;
}

void Chunk::SetBlock(const glm::ivec3& pos, EBlock block)
{
	if (IsPosValid(pos) == false)
		return;

	// TODO: Set block around block dirty
	m_Blocks[pos.x][pos.z][pos.y] = block;
	m_RenderedBlocks[pos] = { true,{} };

	glm::ivec3 dirs[] = {{ -1,0,0 }, { 1,0,0 }, { 0,1,0 }, { 0,-1,0 }, { 0,0,1 }, { 0,0,-1 }};
	for (const auto& dir : dirs)
	{
		auto posToCheck = pos + dir;
		if (m_RenderedBlocks.contains(posToCheck))
			m_RenderedBlocks.at(posToCheck).first = true;

		if (IsPosValid(posToCheck) 
			&& m_Blocks[posToCheck.x][posToCheck.z][posToCheck.y] != EBlock::air)
			m_RenderedBlocks[posToCheck] = { true, {} };
	}

	m_IsDirty = true;
}

std::pair<std::vector<real::PosTexNorm>, std::vector<uint32_t>> Chunk::CalculateMeshData()
{
	auto& blockParser = BlockParser::GetInstance();

	std::vector<real::PosTexNorm> vertices;
	std::vector<uint32_t> indices;

	constexpr glm::vec3 dirs[6] = { {0,0,-1},{1,0,0},{0,0,1},{-1,0,0},{0,1,0},{0,-1,0} };

	std::vector<glm::vec3> blocksToRemove;

	for (auto& [pos, data] : m_RenderedBlocks)
	{
		const auto block = m_Blocks[static_cast<float>(pos.x)][static_cast<float>(pos.z)][static_cast<float>(pos.y)];
		if (block == EBlock::air)
		{
			blocksToRemove.push_back(pos);
			continue;
		}

		if (blockParser.IsTransparent(block))
			continue;

		if (data.first)
		{
			data.second.clear();

			int counter = 0;
			for (int i = 0; i < static_cast<int>(EDirection::amountOfDirections); ++i)
			{
				const auto dir = static_cast<EDirection>(i);
				const auto dirOffset = dirs[i];

				const glm::ivec3 posToCheck = pos + dirOffset;

				if (CanRenderFace(block, posToCheck.x, posToCheck.z, posToCheck.y) == false)
					continue;

				++counter;

				const int offset = static_cast<int>(vertices.size());
				BlockParser::vertices_and_indices p;
				p = blockParser.GetFaceData(dir, block, pos, offset);

				vertices.insert(vertices.end(), p.first.begin(), p.first.end());
				data.second.insert(data.second.end(), p.first.begin(), p.first.end());

				indices.insert(indices.end(), p.second.begin(), p.second.end());
			}

			if (counter == 0)
				blocksToRemove.push_back(pos);

			data.first = false;
		}
		else
		{
			for (int i = 0; i < data.second.size(); i += 4)
			{
				const auto offset = static_cast<uint32_t>(vertices.size());
				vertices.insert(vertices.end(), data.second.begin() + i, data.second.begin() + (i + 4));
				indices.insert(indices.end(), { 0 + offset, 1 + offset, 2 + offset, 2 + offset, 3 + offset, 0 + offset });
			}
		}
	}

	std::ranges::for_each(blocksToRemove, [this](const glm::vec3& pos) { m_RenderedBlocks.erase(pos); });

	return { vertices, indices };
}

std::vector<TransparentFace> Chunk::CalculateTransparentMeshData()
{
	auto& blockParser = BlockParser::GetInstance();

	std::vector<TransparentFace> faces;

	constexpr glm::vec3 dirs[6] = { {0,0,-1},{1,0,0},{0,0,1},{-1,0,0},{0,1,0},{0,-1,0} };

	std::vector<glm::vec3> blocksToRemove;

	for (auto& [pos, data] : m_RenderedBlocks)
	{
		const auto block = m_Blocks[pos.x][pos.z][pos.y];
		if (block == EBlock::air)
		{
			blocksToRemove.push_back(pos);
			continue;
		}

		if (blockParser.IsTransparent(block) == false)
			continue;

		if (data.first)
		{
			data.second.clear();

			int counter = 0;
			for (int i = 0; i < static_cast<int>(EDirection::amountOfDirections); ++i)
			{
				const auto dir = static_cast<EDirection>(i);
				const auto dirOffset = dirs[i];

				auto accuPos = /*chunkPos + */pos;

				if (CanRenderFace(block, pos.x + dirOffset.x, pos.z + dirOffset.z, pos.y + dirOffset.y) == false)
					continue;

				++counter;

				std::vector<real::PosTexNorm> v;
				if (block == EBlock::water)
					v = FluidParser::GetInstance().GetFaceData(dir, accuPos, m_Blocks[pos.x][pos.z][pos.y + 1] == EBlock::water);
				else
					v = blockParser.GetFaceData(dir, block, accuPos, 0).first;

				if (v.empty())
					continue;

				data.second.insert(data.second.end(), v.begin(), v.end());

				auto type = TransparencyType::none;
				std::array vertices{ v[0],v[1], v[2], v[3] };

				if (block == EBlock::water)
					type = TransparencyType::water;
				else if (blockParser.IsCrossBlock(block))
					type = TransparencyType::transparentSprite;
				else
					type = TransparencyType::transparentTexture;

				faces.emplace_back(vertices, type);
			}

			if (counter == 0)
				blocksToRemove.push_back(pos);

			data.first = false;
		}
		else
		{
			for (int i = 0; i < data.second.size(); i += 4)
			{
				auto type = TransparencyType::none;
				std::array vertices{ data.second[i],data.second[i + 1], data.second[i + 2], data.second[i + 3] };

				if (block == EBlock::water)
					type = TransparencyType::water;
				else if (blockParser.IsCrossBlock(block))
					type = TransparencyType::transparentSprite;
				else
					type = TransparencyType::transparentTexture;

				faces.emplace_back(vertices, type);
			}
		}
	}

	std::ranges::for_each(blocksToRemove, [this](const glm::vec3& pos) { m_RenderedBlocks.erase(pos); });

	return faces;
}

bool Chunk::CanRenderFace(EBlock currentBlock, int x, int z, int y) const
{
	auto& blockParser = BlockParser::GetInstance();

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
		const auto pOtherChunk = GetAdjacentChunk({x,y,z});

		if (pOtherChunk == nullptr)
			return false;

		const int blockX = (x < 0) ? CHUNK_SIZE - 1 : (x >= CHUNK_SIZE) ? 0 : x;
		const int blockZ = (z < 0) ? CHUNK_SIZE - 1 : (z >= CHUNK_SIZE) ? 0 : z;

		const auto otherBlock = pOtherChunk->m_Blocks[blockX][blockZ][y];
		return otherBlock == EBlock::air
			|| blockParser.IsTransparent(currentBlock) && !blockParser.IsTransparent(otherBlock)
			|| !blockParser.IsTransparent(currentBlock) && blockParser.IsTransparent(otherBlock)
			|| blockParser.IsTransparent(currentBlock) && blockParser.IsTransparent(otherBlock) && (otherBlock != currentBlock || otherBlock == EBlock::oakLeaves && currentBlock ==EBlock::oakLeaves);
	}
#endif // SINGLE_CHUNK

	const auto otherBlock = m_Blocks[x][z][y];
	return otherBlock == EBlock::air
		|| blockParser.IsTransparent(currentBlock) && !blockParser.IsTransparent(otherBlock)
		|| !blockParser.IsTransparent(currentBlock) && blockParser.IsTransparent(otherBlock)
		|| blockParser.IsTransparent(currentBlock) && blockParser.IsTransparent(otherBlock) && (otherBlock != currentBlock || otherBlock == EBlock::oakLeaves && currentBlock ==EBlock::oakLeaves);
}

void Chunk::InitSolidChunk(const real::GameContext& context)
{
	const auto id = real::GameTime::GetInstance().StartTimer();
	auto [vertices, indices] = CalculateMeshData();
	auto time = real::GameTime::GetInstance().EndTimer(id);

	real::MeshInfo info;
	info.vertexCapacity = static_cast<uint32_t>(vertices.size());
	info.indexCapacity = static_cast<uint32_t>(indices.size());
	info.usesUbo = true;
	info.texture = real::ContentManager::GetInstance().LoadTexture(context, "Resources/textures/atlas.png");

	auto& go = GetOwner()->CreateGameObject();
	m_pSolidMeshComponent = go.AddComponent<real::MeshIndexed<real::PosTexNorm, real::UniformBufferObject>>(info);
	const auto pMat = real::MaterialManager::GetInstance().GetMaterial<DiffuseMaterial>();
	m_pSolidMeshComponent->SetMaterial(pMat);

	m_pSolidMeshComponent->SetVertices(vertices);
	m_pSolidMeshComponent->SetIndices(indices);
	m_pSolidMeshComponent->Init(real::RealEngine::GetGameContext());
}

void Chunk::InitTransparentChunk()
{
	const auto id = real::GameTime::GetInstance().StartTimer();
	const auto faces = CalculateTransparentMeshData();
	auto time = real::GameTime::GetInstance().EndTimer(id);

	auto& go = GetOwner()->CreateGameObject();
	m_pTransparentMeshComponent = go.AddComponent<TransparentModel>(static_cast<uint32_t>((faces.size() * 4) * 2), static_cast<uint32_t>((faces.size() * 6) * 2));
	m_pTransparentMeshComponent->AddFaces(faces);

	glm::ivec2 v{ 8,8 };
	const auto worldPos = GetOwner()->GetTransform()->GetWorldPosition();
	if (worldPos.x < 0) v.x = 15;
	else if (worldPos.x > 0) v.x = 0;
	if (worldPos.z < 0) v.y = 0;
	else if (worldPos.z > 0) v.y = 15;

	m_pTransparentMeshComponent->SortFaces({ v.x,75,v.y }, m_ChunkIsCenter);
}

void Chunk::GenerateTree(const glm::ivec3& pos)
{
	{
		constexpr int x = 1;
		constexpr int y = 50;

		if (rand() % y > x)
			return;
	}

	using leaves_for_other_chunk = std::unordered_map<glm::ivec2, std::vector<std::pair<glm::ivec3, EBlock>>>;
	leaves_for_other_chunk map;

	constexpr glm::ivec2 dirs[] = { glm::ivec2{1, 0}/*, glm::ivec2{1, 1}*/, glm::ivec2{0, 1}/*, glm::ivec2{-1, 1}*/, glm::ivec2{-1, 0}/*, glm::ivec2{-1, -1}*/,
		glm::ivec2{0, -1}/*, glm::ivec2{1, -1} */};
	constexpr glm::ivec2 dirsExtended[] = {
		glm::ivec2{1, 0}, glm::ivec2{1, 1}, glm::ivec2{0, 1}, glm::ivec2{-1, 1}, glm::ivec2{-1, 0}, glm::ivec2{-1, -1},
		glm::ivec2{0, -1}, glm::ivec2{1, -1}, glm::ivec2{2, 0}, glm::ivec2{2, 1}, glm::ivec2{2, 2}, glm::ivec2{1, 2},
		glm::ivec2{0, 2}, glm::ivec2{-1, 2}, glm::ivec2{-2, 2}, glm::ivec2{-2, 1}, glm::ivec2{-2, 0},
		glm::ivec2{-2, -1}, glm::ivec2{-2, -2}, glm::ivec2{-1, -2}, glm::ivec2{0, -2}, glm::ivec2{1, -2},
		glm::ivec2{2, -2}, glm::ivec2{2, -1},
	};

	// Only plant trees if there is no other tree in a 2 block radius
	for (const auto& dir : dirsExtended)
	{
		auto x = pos.x + dir.x;
		auto z = pos.z + dir.y;

		if (x < CHUNK_SIZE && x >= 0
			&& z < CHUNK_SIZE && z >= 0)
		{
			if (m_Blocks[x][z][pos.y + 2] != EBlock::air)
				return;
		}
		else
		{
			if (const auto pOtherChunk = GetAdjacentChunk({ x, pos.y, z }))
			{
				const int blockX = (x < 0) ? CHUNK_SIZE - 1 : (x >= CHUNK_SIZE) ? 0 : x;
				const int blockZ = (z < 0) ? CHUNK_SIZE - 1 : (z >= CHUNK_SIZE) ? 0 : z;
				if (pOtherChunk->m_Blocks[blockX][blockZ][pos.y + 2] != EBlock::air)
					return;
			}
		}
	}

	constexpr int maxHeight = 5;
	constexpr int minHeight = 4;
	int height = 0;
	for (int i = 1; i <= maxHeight; ++i)
	{
		if (i > minHeight)
		if (rand() % 2 == 1)
			break;

		++height;
		m_Blocks[pos.x][pos.z][pos.y + i] = EBlock::oakLog;
	}

	const int leaveStart = height - 2;
	const auto setLeavesBlock = [pos, leaveStart, &map, this](int i, const glm::ivec2& dir) {
		const auto x = pos.x + dir.x;
		const auto y = pos.y + leaveStart + i;
		const auto z = pos.z + dir.y;

		if (x < CHUNK_SIZE && x >= 0
			&& z < CHUNK_SIZE && z >= 0)
		{
			m_Blocks[x][z][y] = EBlock::oakLeaves;
		}
		else
		{
			const auto otherChunkPos = GetAdjacentChunkPos({ x, y, z });

			const int blockX = (x < 0) ? CHUNK_SIZE + x : (x >= CHUNK_SIZE) ? x % CHUNK_SIZE : x;
			const int blockZ = (z < 0) ? CHUNK_SIZE + z : (z >= CHUNK_SIZE) ? z % CHUNK_SIZE : z;

			if (const auto pOtherChunk = GetAdjacentChunk({ x, y, z }))
			{
				if (pOtherChunk->m_Blocks[blockX][blockZ][y] == EBlock::air)
				{
					pOtherChunk->m_Blocks[blockX][blockZ][y] = EBlock::oakLeaves;
					pOtherChunk->m_RenderedBlocks[{blockX, y, blockZ}] = { true, {} };
					pOtherChunk->m_IsDirty = true;
				}
			}
			else
			{
				map[otherChunkPos].push_back({ {blockX, y, blockZ}, EBlock::oakLeaves });
			}
		}
		};

	for (int i = 0; i < 4; ++i)
	{
		if (i < 2)
		{
			for (const auto& dir : dirsExtended)
			{
				setLeavesBlock(i, dir);
			}
		}
		else
		{
			for (const auto& dir : dirs)
			{
				setLeavesBlock(i, dir);
			}

			if (i > 2)
			{
				m_Blocks[pos.x][pos.z][pos.y + leaveStart + i] = EBlock::oakLeaves;
			}
		}
	}

	if (map.empty() == false)
	{
		for (const auto& [chunkPos, blocks] : map)
		{
			m_pWorldComponent->AddBlocksForFutureChunks(chunkPos, blocks);
		}
	}

	m_HighestY = std::max(pos.y + maxHeight + 1, m_HighestY);
}

void Chunk::GenerateFlower(const glm::ivec3& pos)
{
	if (m_Blocks[pos.x][pos.z][pos.y + 1] != EBlock::air)
		return;

	{
		constexpr int x = 1;
		constexpr int y = 20;

		if (rand() % y > x)
			return;
	}
	bool poppy;
	{
		constexpr int x = 1;
		constexpr int y = 2;

		poppy = rand() % y == x;
	}

	m_Blocks[pos.x][pos.z][pos.y + 1] = poppy ? EBlock::poppy : EBlock::dandelion;
}

glm::ivec2 Chunk::GetAdjacentChunkPos(glm::vec3 outOfScopePos) const
{
	const auto pos = GetOwner()->GetTransform()->GetWorldPosition();
	auto chunkPos = glm::vec2(pos.x, pos.z);

	if (outOfScopePos.x < 0)
		chunkPos.x -= CHUNK_SIZE;
	else if (outOfScopePos.x >= CHUNK_SIZE)
		chunkPos.x += CHUNK_SIZE;

	if (outOfScopePos.z < 0)
		chunkPos.y -= CHUNK_SIZE;
	else if (outOfScopePos.z >= CHUNK_SIZE)
		chunkPos.y += CHUNK_SIZE;

	return chunkPos;
}

Chunk* Chunk::GetAdjacentChunk(glm::vec3 outOfScopePos) const
{
	return m_pWorldComponent->GetChunkAt(GetAdjacentChunkPos(outOfScopePos));
}

bool Chunk::IsPosValid(const glm::vec3& pos)
{
	return pos.x >= 0 && pos.x < CHUNK_SIZE
		&& pos.z >= 0 && pos.z < CHUNK_SIZE
		&& pos.y >= 0 && pos.y < CHUNK_HEIGHT;
}
