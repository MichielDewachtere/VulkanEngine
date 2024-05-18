#ifndef BLOCKPARSER_H
#define BLOCKPARSER_H

#include <unordered_map>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>
#include <real_core/Singleton.h>

#include <Util/Structs.h>

#include "Enumerations.h"
#include "BlockModel.h"
#include "GameUtils.h"
//#include "bimap.hpp"

class BlockParser final : public real::Singleton<BlockParser>
{
public:
	~BlockParser() override = default;

	BlockParser(const BlockParser& other) = delete;
	BlockParser& operator=(const BlockParser& rhs) = delete;
	BlockParser(BlockParser&& other) = delete;
	BlockParser& operator=(BlockParser&& rhs) = delete;

	std::vector<real::PosTexNorm> GetFaceData(EDirection dir, EBlock block, glm::vec3 pos);

private:
	friend class Singleton<BlockParser>;
	explicit BlockParser();

	int m_AmountOfTexX{ 8 }, m_AmountOfTexY{ 8 }, m_TextureSize{ 16 }, m_AtlasSizeX{ 128 }, m_AtlasSizeY{ 128 };
	float m_NormalTextureWidth{}, m_NormalTextureHeight{};
	std::unordered_map<EBlock, BlockModel> m_Blocks;
	std::unordered_map<EBlockType, BlockModel> m_BlockTypes;

	static std::vector<glm::vec3> GetVertexPositions(const BlockElement& e, EDirection dir);
	glm::vec2 GetTexCoord(int atlasId, int vertexId, EDirection dir, BlockModel model) const;

	void ParseBlock(EBlock block);
	void FillElement(BlockModel& model, size_t i, const nlohmann::basic_json<>& json) const;
	static void FillParent(BlockModel& model, const std::string& parent);

	static inline const std::unordered_map<EBlock, std::string> block_to_name{
		{ EBlock::air, "air" },
		{ EBlock::stone, "stone" },
		{ EBlock::dirt, "dirt" },
		{ EBlock::grassBlock, "grass_block" },
		{ EBlock::debug, "debug_block" },
		{ EBlock::oakLog, "oak_log" },
		{ EBlock::sand, "sand" },
		{ EBlock::waterTemp, "water_temp" },
		{ EBlock::glass, "glass" },
	};

	static inline const std::unordered_map<std::string, EBlockType> name_to_block_type{
		{ "block", EBlockType::block },
		{ "cube", EBlockType::cube },
		{ "cube_all", EBlockType::cubeAll },
		{ "cube_bottom_top", EBlockType::cubeBottomTop },
		{ "cube_column", EBlockType::cubeColumn },
		{ "fluid_temp", EBlockType::fluidTemp},
	};

	static inline const std::unordered_map<EDirection, glm::vec3> direction_to_normal{
	{EDirection::north,{ 0,0,-1 }},
	{EDirection::east,{ 1,0,0 }},
	{EDirection::south,	{ 0,0,1 }},
	{EDirection::west,	{ -1,0,0 }},
	{EDirection::up,	{ 0,1,0 }},
	{EDirection::down,{ 0,-1,0 }} };

	using TextureIndexMap = std::unordered_map<std::pair<EBlockType, EDirection>, int>;
	static inline const TextureIndexMap texture_index_map{
		// cube
		{{EBlockType::cube, EDirection::north}, 0},
		{{EBlockType::cube, EDirection::east}, 1},
		{{EBlockType::cube, EDirection::south}, 2},
		{{EBlockType::cube, EDirection::west}, 3},
		{{EBlockType::cube, EDirection::up}, 4},
		{{EBlockType::cube, EDirection::down}, 5},

		// cubeAll
		{{EBlockType::cubeAll, EDirection::north}, 0},
		{{EBlockType::cubeAll, EDirection::east}, 0},
		{{EBlockType::cubeAll, EDirection::south}, 0},
		{{EBlockType::cubeAll, EDirection::west}, 0},
		{{EBlockType::cubeAll, EDirection::up}, 0},
		{{EBlockType::cubeAll, EDirection::down}, 0},

		// cubeBottomTop
		{{EBlockType::cubeBottomTop, EDirection::north}, 0},
		{{EBlockType::cubeBottomTop, EDirection::east}, 0},
		{{EBlockType::cubeBottomTop, EDirection::south}, 0},
		{{EBlockType::cubeBottomTop, EDirection::west}, 0},
		{{EBlockType::cubeBottomTop, EDirection::up}, 2},
		{{EBlockType::cubeBottomTop, EDirection::down}, 1},

		// cubeColumn
		{{EBlockType::cubeColumn, EDirection::north}, 0},
		{{EBlockType::cubeColumn, EDirection::east}, 0},
		{{EBlockType::cubeColumn, EDirection::south}, 0},
		{{EBlockType::cubeColumn, EDirection::west}, 0},
		{{EBlockType::cubeColumn, EDirection::up}, 1},
		{{EBlockType::cubeColumn, EDirection::down}, 1},
	};
};

#endif // BLOCKPARSER_H