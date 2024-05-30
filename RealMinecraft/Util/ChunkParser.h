#ifndef CHUNKPARSER_H
#define CHUNKPARSER_H

#include <array>
#include <cstdint>
#include <string>
#include <fstream>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <real_core/Singleton.h>

#include "Enumerations.h"
#include "Macros.h"


class ChunkParser final : public real::Singleton<ChunkParser>
{
public:
	~ChunkParser() override;

	ChunkParser(const ChunkParser& other) = delete;
	ChunkParser& operator=(const ChunkParser& rhs) = delete;
	ChunkParser(ChunkParser&& other) = delete;
	ChunkParser& operator=(ChunkParser&& rhs) = delete;

	void Init(uint32_t seed);

	using block_array = std::array<std::array<std::array<EBlock, CHUNK_HEIGHT>, CHUNK_SIZE>, CHUNK_SIZE>;
	void LoadChunk(const glm::ivec2& chunkPos, block_array& blocks, int& highestY, int& lowestY) const;

	// TODO: Thread this?
	void SaveBlock(const glm::ivec2& chunkPos, const glm::ivec3& blockPos, EBlock block);
	bool HasChunkData(const glm::ivec2& chunkPos) const;

private:
	struct BlockChange
	{
		uint8_t y;
		uint8_t x : 4;
		uint8_t z : 4;
		EBlock type;

		uint16_t EncodePosition() const
		{
			return (y << 8) | (x << 4) | z;
		}

		static BlockChange Decode(uint16_t pos, uint8_t type)
		{
			BlockChange change;
			change.y = pos >> 8;
			change.x = (pos >> 4) & 0xF;
			change.z = pos & 0xF;
			change.type = static_cast<EBlock>(type);
			return change;
		}
	};

	friend class Singleton<ChunkParser>;
	explicit ChunkParser() = default;

	uint32_t m_Seed{ 0 };
	std::string m_Path{};

	std::pair<glm::ivec2, std::ofstream> m_OpenFile;

	std::string CreateFileName(const glm::ivec2& chunkPos) const;
};

#endif // CHUNKPARSER_H