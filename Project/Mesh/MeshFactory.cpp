#include "MeshFactory.h"

std::pair<std::vector<uint32_t>, std::vector<PosColNorm>> MeshFactory::CreateCube(const glm::vec3& pos, float size)
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
	std::vector<uint32_t> indices;

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

std::pair<std::vector<uint32_t>, std::vector<PosTexNorm>> MeshFactory::CreateCubeMap(const glm::vec3& pos, float size)
{
	constexpr glm::vec2 bottomLeft = { 0,0};
	constexpr glm::vec2 bottomRight = { 1,0 };
	constexpr glm::vec2 topLeft = { 0,1 };
	constexpr glm::vec2 topRight = { 1,1 };

	constexpr glm::vec3 front = { 0,0,1 };
	constexpr glm::vec3 back = { 0,0,-1 };
	constexpr glm::vec3 left = { -1,0,0 };
	constexpr glm::vec3 right = { 1,0,0 };
	constexpr glm::vec3 top = { 0,1,0 };
	constexpr glm::vec3 bottom = { 0,-1,0 };

	std::vector<PosTexNorm> data;
	std::vector<uint32_t> indices;

	data.push_back({ pos, topLeft, front });
	data.push_back({ {pos.x + 1, pos.y, pos.z}, topRight, front });
	data.push_back({ {pos.x + 1, pos.y + 1, pos.z}, bottomRight, front });
	data.push_back({ {pos.x, pos.y + 1, pos.z}, bottomLeft, front });
	indices.insert(indices.end(), { 0, 1, 2, 2, 3, 0 });

	data.push_back({ {pos.x, pos.y + 1, pos.z - 1}, bottomLeft, back });
	data.push_back({ {pos.x + 1, pos.y + 1, pos.z - 1}, bottomRight, back });
	data.push_back({ {pos.x + 1, pos.y, pos.z - 1}, topRight, back });
	data.push_back({ {pos.x, pos.y, pos.z - 1}, topLeft, back });
	indices.insert(indices.end(), { 4, 5, 6, 6, 7, 4 });

	data.push_back({ {pos.x, pos.y, pos.z - 1}, topLeft, left });
	data.push_back({ {pos.x, pos.y, pos.z}, topRight, left });
	data.push_back({ {pos.x, pos.y + 1, pos.z}, bottomRight, left });
	data.push_back({ {pos.x, pos.y + 1, pos.z - 1}, bottomLeft, left });
	indices.insert(indices.end(), { 8, 9, 10, 10, 11, 8 });

	data.push_back({ {pos.x + 1, pos.y, pos.z}, topLeft, right });
	data.push_back({ {pos.x + 1, pos.y, pos.z - 1}, topRight, right });
	data.push_back({ {pos.x + 1, pos.y + 1, pos.z - 1}, bottomRight, right });
	data.push_back({ {pos.x + 1, pos.y + 1, pos.z}, bottomLeft, right });
	indices.insert(indices.end(), { 12, 13, 14, 14, 15, 12 });

	data.push_back({ {pos.x, pos.y + 1, pos.z}, topLeft, top });
	data.push_back({ {pos.x + 1, pos.y + 1, pos.z}, topRight, top });
	data.push_back({ {pos.x + 1, pos.y + 1, pos.z - 1}, bottomRight, top });
	data.push_back({ {pos.x, pos.y + 1, pos.z - 1}, bottomLeft, top });
	indices.insert(indices.end(), { 16, 17, 18, 18, 19, 16 });

	data.push_back({ {pos.x, pos.y, pos.z - 1}, topLeft, bottom });
	data.push_back({ {pos.x + 1, pos.y, pos.z - 1}, topRight, bottom });
	data.push_back({ {pos.x + 1, pos.y, pos.z}, bottomRight, bottom });
	data.push_back({ {pos.x, pos.y, pos.z}, bottomLeft, bottom });
	indices.insert(indices.end(), { 20, 21, 22, 22, 23, 20 });

	return std::make_pair(indices, data);
}