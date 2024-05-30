#ifndef CHUNK_H
#define CHUNK_H

#include <array>
#include <real_core/Component.h>

#include "TransparentModel.h"
#include "World.h"
#include "Mesh/MeshIndexed.h"
#include "Misc/AABB.h"
#include "Util/Macros.h"

class World;
enum class EDirection : char;
enum class EBlock;

class Chunk final : public real::Component
{
public:
	explicit Chunk(real::GameObject* pOwner, const std::vector<std::pair<glm::ivec3, EBlock>>& blocks = {});
	~Chunk() override = default;

	Chunk(const Chunk& other) = delete;
	Chunk& operator=(const Chunk& rhs) = delete;
	Chunk(Chunk&& other) = delete;
	Chunk& operator=(Chunk&& rhs) = delete;

	virtual void Start() override;
	virtual void Update() override;

	void UpdateChunkBoarder(const Chunk* adjacentChunk, const glm::ivec2& dir);
	void SortBlocks(const glm::ivec3& position) const;

	void SetAsCenter(bool isCenter) { m_ChunkIsCenter = isCenter; }

	bool IsBlockAir(const glm::ivec3& pos) const;
	bool IsBlockWater(const glm::ivec3& pos) const;
	void SetBlock(const glm::ivec3& pos, EBlock block);

private:
	bool m_IsDirty{ false }, m_ChunkIsCenter{ false };

	int m_LowestY{ CHUNK_HEIGHT }, m_HighestY{ 0 };
	real::AABB m_Aabb{};

	float m_BlockRemoveTime{ 2.f }, m_AccuTime{ 0.f };
	size_t m_RemoveBlock{ 63 }, m_AddBlock{ 64 };

	using block_array = std::array<std::array<std::array<EBlock, CHUNK_HEIGHT>, CHUNK_SIZE>, CHUNK_SIZE>;
	block_array m_Blocks{};
	std::map<glm::vec3, std::pair<bool, std::vector<real::PosTexNorm>>, VecComparator<3, float>> m_RenderedBlocks{};
	//std::map < glm::vec3, std::pair<EBlock>> m_ChangedBlocks;

	real::MeshIndexed<real::PosTexNorm, real::UniformBufferObject>* m_pSolidMeshComponent{ nullptr };

	TransparentModel* m_pTransparentMeshComponent{ nullptr };

	World* m_pWorldComponent{ nullptr };

	std::pair<std::vector<real::PosTexNorm>, std::vector<uint32_t>> CalculateMeshData();
	std::vector<TransparentFace> CalculateTransparentMeshData();

	bool CanRenderFace(EBlock currentBlock, int x, int z, int y) const;

	void InitSolidChunk(const real::GameContext& context);

	void InitTransparentChunk();

	void GenerateTree(const glm::ivec3& pos);
	void GenerateFlower(const glm::ivec3& pos);

	glm::ivec2 GetAdjacentChunkPos(glm::vec3 outOfScopePos) const;
	Chunk* GetAdjacentChunk(glm::vec3 outOfScopePos) const;

	static bool IsPosValid(const glm::vec3& pos);

	static int GetDeterministicRandomNumber(int seed, const glm::ivec3& position, int lowerBound, int upperBound);
	static inline void HashCombine(std::size_t& seed, std::size_t value) {
		seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}
};

#endif // CHUNK_H