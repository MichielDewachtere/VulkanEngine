#ifndef BLOCKMODEL_H
#define BLOCKMODEL_H

struct BlockFace
{
	//				x1   y1    x2    y2
	glm::vec4 uv{ 0,0,16,16 };
	std::array<real::PosTexNorm, 4> vertices;
	bool isFull{ true };
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
	bool transparent{ false }, fullBlock{ true };
};


#endif // BLOCKMODEL_H