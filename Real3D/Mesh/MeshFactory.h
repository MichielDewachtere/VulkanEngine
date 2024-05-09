#ifndef MESHFACTORY_H
#define MESHFACTORY_H

#include <utility>
#include <vector>

#include "Util/Structs.h"

namespace real
{
	class MeshFactory
	{
	public:
		static std::pair<std::vector<uint32_t>, std::vector<PosColNorm>> CreatePyramid(const glm::vec3& pos, float height, float baseSize);

		static std::pair<std::vector<uint32_t>, std::vector<PosColNorm>> CreateCube(const glm::vec3& pos, float size);
		static std::pair<std::vector<uint32_t>, std::vector<PosTexNorm>> CreateCubeMap(const glm::vec3& pos, float size);
		//static std::pair<std::vector<uint16_t>, std::vector<PosTexNorm>> CreateCubeMap(const glm::vec2& pos, float size);
	};
}

#endif // MESHFACTORY_H