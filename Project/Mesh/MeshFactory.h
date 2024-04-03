#ifndef MESHFACTORY_H
#define MESHFACTORY_H

#include <utility>
#include <vector>

#include "Util/Structs.h"

class MeshFactory
{
public:
	static std::pair<std::vector<uint16_t>, std::vector<PosColNorm>> CreateCube(const glm::vec3& pos, float size);
	static std::pair<std::vector<uint16_t>, std::vector<PosColNorm>> CreateCubeMap(const glm::vec3& pos, float size);
	//static std::pair<std::vector<uint16_t>, std::vector<PosTexNorm>> CreateCubeMap(const glm::vec2& pos, float size);
};

#endif // MESHFACTORY_H