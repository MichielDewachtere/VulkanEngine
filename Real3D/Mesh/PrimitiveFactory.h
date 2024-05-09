#ifndef PRIMITIVEFACTORY_H
#define PRIMITIVEFACTORY_H

#include <vector>

#include "Util/Structs.h"

namespace real
{
	class PrimitiveFactory
	{
	public:
		static std::pair<std::vector<uint16_t>, std::vector<PosCol2D>> CreateSquare(const glm::vec2& pos, float size);
		static std::pair<std::vector<uint16_t>, std::vector<PosCol2D>> CreateRectangle(const glm::vec2& pos, float sizeX, float sizeY);
		static std::pair<std::vector<uint16_t>, std::vector<PosCol2D>> CreateCircle(const glm::vec2& pos, float radius, int numOfVertices);
		static std::pair<std::vector<uint16_t>, std::vector<PosCol2D>> CreateEllipse(const glm::vec2& pos, float radiusX, float radiusY, int numOfVertices);
	};
}

#endif // PRIMITIVEFACTORY_H