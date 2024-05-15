#ifndef CHUNK_H
#define CHUNK_H

#include <array>
#include <real_core/Component.h>

#include "Mesh/MeshIndexed.h"
#include "Util/Macros.h"

enum class EDirection : char;
enum class EBlock;

// Custom comparator for glm::vec3
struct Vec3Comparator {
	bool operator()(const glm::vec3& lhs, const glm::vec3& rhs) const {
		if (lhs.x < rhs.x) return true;
		if (lhs.x > rhs.x) return false;
		if (lhs.y < rhs.y) return true;
		if (lhs.y > rhs.y) return false;
		return lhs.z < rhs.z;
	}
};

class Chunk final : public real::Component
{
public:
	explicit Chunk(real::GameObject* pOwner);
	~Chunk() override = default;

	Chunk(const Chunk& other) = delete;
	Chunk& operator=(const Chunk& rhs) = delete;
	Chunk(Chunk&& other) = delete;
	Chunk& operator=(Chunk&& rhs) = delete;

	virtual void Start() override;
	virtual void Update() override;

private:
	bool m_IsDirty{ false };

	float m_BlockRemoveTime{ 2.f }, m_AccuTime{ 0.f };
	size_t m_RemoveBlock{ 63 }, m_AddBlock{ 64 };

	std::array<std::array<std::array<EBlock, CHUNK_HEIGHT>, CHUNK_SIZE>, CHUNK_SIZE> m_Blocks{};
	std::map<glm::vec3, std::pair<bool, std::vector<real::PosTexNorm>>, Vec3Comparator> m_RenderedBlocks{};

	real::MeshIndexed<real::PosTexNorm>* m_pMeshComponent{ nullptr };

	std::pair<std::vector<real::PosTexNorm>, std::vector<uint32_t>> CalculateMeshData();

	bool CanRenderFace(size_t x, size_t z, size_t y) const;
};

#endif // CHUNK_H