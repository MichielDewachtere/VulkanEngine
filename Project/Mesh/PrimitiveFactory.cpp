#include "PrimitiveFactory.h"

std::pair<std::vector<uint16_t>, std::vector<PosCol2D>> PrimitiveFactory::CreateSquare(const glm::vec2& pos, float size)
{
	return CreateRectangle(pos, size, size);
}

std::pair<std::vector<uint16_t>, std::vector<PosCol2D>> PrimitiveFactory::CreateRectangle(const glm::vec2& pos, float sizeX, float sizeY)
{
	const std::vector rectVertices =
	{
		PosCol2D{ glm::vec2{pos.x - sizeX / 2.f, pos.y - sizeX / 2.f}, glm::vec3{1,0,0} },
		PosCol2D{ glm::vec2{pos.x + sizeX / 2.f, pos.y - sizeX / 2.f}, glm::vec3{0,1,0} },
		PosCol2D{ glm::vec2{pos.x + sizeX / 2.f, pos.y + sizeX / 2.f}, glm::vec3{0,0,1} },
		PosCol2D{ glm::vec2{pos.x - sizeX / 2.f, pos.y + sizeX / 2.f}, glm::vec3{1,1,1} },
	};

	const std::vector<uint16_t> indices =
	{
		0, 1, 2, 2, 3, 0
	};

	return std::make_pair(indices, rectVertices);
}

std::pair<std::vector<uint16_t>, std::vector<PosCol2D>> PrimitiveFactory::CreateCircle(const glm::vec2& pos, float radius, int numOfVertices)
{
	return CreateEllipse(pos, radius, radius, numOfVertices);
}

std::pair<std::vector<uint16_t>, std::vector<PosCol2D>> PrimitiveFactory::CreateEllipse(const glm::vec2& /*pos*/, float /*radiusX*/, float /*radiusY*/, int /*numOfVertices*/)
{
	return {};
}
