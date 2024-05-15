#ifndef GAMESTRUCTS_H
#define GAMESTRUCTS_H

struct BlockFace
{
	//EDirection direction{};
	//				x1   y1    x2    y2
	glm::vec4 uv{ 0,0,16,16 };
};

struct BlockElement
{
	glm::vec3 from{ -1 }, to{ -1 };
	std::map<EDirection, BlockFace> faces{};
};

struct BlockModel
{
	EBlockType parent{ EBlockType::block };
	std::vector<int> textures{};
	std::vector<BlockElement> elements{};
};


#endif // GAMESTRUCTS_H