#include "MeshFactory.h"

std::pair<std::vector<uint16_t>, std::vector<PosColNorm>> MeshFactory::CreateCube(const glm::vec3& pos, float size)
{
	constexpr glm::vec3 red = { 1,0,0 };
	constexpr glm::vec3 green = { 0,1,0 };
	constexpr glm::vec3 blue = { 0,0,1 };

	constexpr glm::vec3 front = { 0,0,1 };
	constexpr glm::vec3 back = { 0,0,-1 };
	constexpr glm::vec3 left = { -1,0,0 };
	constexpr glm::vec3 right = { 1,0,0 };
	constexpr glm::vec3 top = { 0,1,0 };
	constexpr glm::vec3 bottom = { 0,-1,0 };

	std::vector<PosColNorm> data;
	std::vector<uint16_t> indices;

	data.push_back({ pos, red, front });
	data.push_back({ {pos.x + 1, pos.y, pos.z}, red, front });
	data.push_back({ {pos.x + 1, pos.y + 1, pos.z}, red, front });
	data.push_back({ {pos.x, pos.y + 1, pos.z}, red, front });
	indices.insert(indices.end(), { 0, 1, 2, 2, 3, 0 });

	data.push_back({ {pos.x, pos.y + 1, pos.z - 1}, red, back });
	data.push_back({ {pos.x + 1, pos.y + 1, pos.z - 1}, red, back });
	data.push_back({ {pos.x + 1, pos.y, pos.z - 1}, red, back });
	data.push_back({ {pos.x, pos.y, pos.z - 1}, red, back });
	indices.insert(indices.end(), { 4, 5, 6, 6, 7, 4 });

	data.push_back({ {pos.x, pos.y, pos.z - 1}, green, left });
	data.push_back({ {pos.x, pos.y, pos.z}, green, left });
	data.push_back({ {pos.x, pos.y + 1, pos.z}, green, left });
	data.push_back({ {pos.x, pos.y + 1, pos.z - 1}, green, left });
	indices.insert(indices.end(), { 8, 9, 10, 10, 11, 8 });

	data.push_back({ {pos.x + 1, pos.y, pos.z}, green, right });
	data.push_back({ {pos.x + 1, pos.y, pos.z - 1}, green, right });
	data.push_back({ {pos.x + 1, pos.y + 1, pos.z - 1}, green, right });
	data.push_back({ {pos.x + 1, pos.y + 1, pos.z}, green, right });
	indices.insert(indices.end(), { 12, 13, 14, 14, 15, 12 });

	data.push_back({ {pos.x, pos.y + 1, pos.z}, blue, top });
	data.push_back({ {pos.x + 1, pos.y + 1, pos.z}, blue, top });
	data.push_back({ {pos.x + 1, pos.y + 1, pos.z - 1}, blue, top });
	data.push_back({ {pos.x, pos.y + 1, pos.z - 1}, blue, top });
	indices.insert(indices.end(), { 16, 17, 18, 18, 19, 16 });

	data.push_back({ {pos.x, pos.y, pos.z - 1}, blue, bottom });
	data.push_back({ {pos.x + 1, pos.y, pos.z - 1}, blue, bottom });
	data.push_back({ {pos.x + 1, pos.y, pos.z}, blue, bottom });
	data.push_back({ {pos.x, pos.y, pos.z}, blue, bottom });
	indices.insert(indices.end(), { 20, 21, 22, 22, 23, 20 });

	return std::make_pair(indices, data);
}

std::pair<std::vector<uint16_t>, std::vector<PosColNorm>> MeshFactory::CreateCubeMap(const glm::vec3& pos, float size)
{
	return {};
}