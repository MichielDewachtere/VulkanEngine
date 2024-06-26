#include "MeshFactory.h"

#include <real_core/Colors.h>

std::pair<std::vector<uint32_t>, std::vector<real::PosColNorm>> real::MeshFactory::CreatePyramid(const glm::vec3& pos, float height, float baseSize)
{
	// Colors
	const glm::vec3 red = real::Colors::red;
	const glm::vec3 green = real::Colors::green;
	const glm::vec3 blue = real::Colors::blue;

	// Positions
	//      5
	//      |
	//    4-|---3
	//   /  |  /
	//	1-----2
	const glm::vec3 p1 = { pos.x,						pos.y,			pos.z };
	const glm::vec3 p2 = { pos.x + baseSize,			pos.y,			pos.z };
	const glm::vec3 p3 = { pos.x + baseSize,			pos.y,			pos.z - baseSize };
	const glm::vec3 p4 = { pos.x,						pos.y,			pos.z - baseSize };
	const glm::vec3 p5 = { pos.x + baseSize / 2.f,pos.y + height,	pos.z - baseSize / 2.f };

	// Normals
	constexpr glm::vec3 nFront = { 0,1,1 };
	constexpr glm::vec3 nBack = { 0,1,-1 };
	constexpr glm::vec3 nLeft = { -1,1,0 };
	constexpr glm::vec3 nRight = { 1,1,0 };
	constexpr glm::vec3 nBottom = { 0,-1,0 };

	std::vector<PosColNorm> data;
	std::vector<uint32_t> indices;

	data.push_back({ p1, red, nFront });
	data.push_back({ p2, red, nFront });
	data.push_back({ p5, red, nFront });
	indices.insert(indices.end(), { 0, 1, 2 });

	data.push_back({ p4, red, nBack });
	data.push_back({ p5, red, nBack });
	data.push_back({ p3, red, nBack });
	indices.insert(indices.end(), { 3, 4, 5 });

	data.push_back({ p5, green, nLeft });
	data.push_back({ p4, green, nLeft });
	data.push_back({ p1, green, nLeft });
	indices.insert(indices.end(), { 6, 7, 8 });

	data.push_back({ p3, green, nRight });
	data.push_back({ p5, green, nRight });
	data.push_back({ p2, green, nRight });
	indices.insert(indices.end(), { 9, 10, 11 });

	data.push_back({ p4, blue, nBottom });
	data.push_back({ p3, blue, nBottom });
	data.push_back({ p2, blue, nBottom });
	data.push_back({ p1, blue, nBottom });
	indices.insert(indices.end(), { 12, 13, 14, 14, 15, 12 });

	return std::make_pair(indices, data);

}

std::pair<std::vector<uint32_t>, std::vector<real::PosColNorm>> real::MeshFactory::CreateCube(const glm::vec3& pos, float size, const glm::u8vec3& color)
{
	const glm::vec3 fColor = glm::vec3(color) / 255.f;

	glm::vec3 leftBot = pos;
	leftBot.z *= -1;
	glm::vec3 rightTop = pos + glm::vec3(size);
	rightTop.z *= -1;

	constexpr glm::vec3 front = { 0,0,1 };
	constexpr glm::vec3 back = { 0,0,-1 };
	constexpr glm::vec3 left = { -1,0,0 };
	constexpr glm::vec3 right = { 1,0,0 };
	constexpr glm::vec3 top = { 0,1,0 };
	constexpr glm::vec3 bottom = { 0,-1,0 };

	std::vector<PosColNorm> data;
	std::vector<uint32_t> indices;

	// Front
	data.emplace_back(glm::vec3{ rightTop.x, leftBot.y, rightTop.z }, fColor, front);
	data.emplace_back(glm::vec3{ leftBot.x, leftBot.y, rightTop.z }, fColor, front);
	data.emplace_back(glm::vec3{ leftBot.x, rightTop.y, rightTop.z }, fColor, front);
	data.emplace_back(glm::vec3{ rightTop.x, rightTop.y, rightTop.z }, fColor, front);
	indices.insert(indices.end(), { 0, 1, 2, 2, 3, 0 });

	// Left
	data.emplace_back(glm::vec3{ rightTop.x, leftBot.y, leftBot.z }, fColor, left);
	data.emplace_back(glm::vec3{ rightTop.x, leftBot.y, rightTop.z }, fColor, left);
	data.emplace_back(glm::vec3{ rightTop.x, rightTop.y, rightTop.z }, fColor, left);
	data.emplace_back(glm::vec3{ rightTop.x, rightTop.y, leftBot.z }, fColor, left);
	indices.insert(indices.end(), { 4, 5, 6, 6, 7, 4 });
	
	// Back
	data.emplace_back(glm::vec3{ leftBot.x, leftBot.y, leftBot.z }, fColor, back);
	data.emplace_back(glm::vec3{ rightTop.x, leftBot.y, leftBot.z }, fColor, back);
	data.emplace_back(glm::vec3{ rightTop.x, rightTop.y, leftBot.z }, fColor, back);
	data.emplace_back(glm::vec3{ leftBot.x, rightTop.y, leftBot.z }, fColor, back);
	indices.insert(indices.end(), { 8, 9, 10, 10, 11, 8 });
	
	// Right
	data.emplace_back(glm::vec3{ leftBot.x, leftBot.y, rightTop.z }, fColor, right);
	data.emplace_back(glm::vec3{ leftBot.x, leftBot.y, leftBot.z }, fColor, right);
	data.emplace_back(glm::vec3{ leftBot.x, rightTop.y, leftBot.z }, fColor, right);
	data.emplace_back(glm::vec3{ leftBot.x, rightTop.y, rightTop.z }, fColor, right);
	indices.insert(indices.end(), { 12, 13, 14, 14, 15, 12 });
	
	// Top
	data.emplace_back(glm::vec3{ leftBot.x, rightTop.y, leftBot.z }, fColor, top);
	data.emplace_back(glm::vec3{ rightTop.x, rightTop.y, leftBot.z }, fColor, top);
	data.emplace_back(glm::vec3{ rightTop.x, rightTop.y, rightTop.z }, fColor, top);
	data.emplace_back(glm::vec3{ leftBot.x, rightTop.y, rightTop.z }, fColor, top);
	indices.insert(indices.end(), { 16, 17, 18, 18, 19, 16 });
	
	// Bottom
	data.emplace_back(glm::vec3{ leftBot.x, leftBot.y, rightTop.z }, fColor, bottom);
	data.emplace_back(glm::vec3{ rightTop.x, leftBot.y, rightTop.z }, fColor, bottom);
	data.emplace_back(glm::vec3{ rightTop.x, leftBot.y, leftBot.z }, fColor, bottom);
	data.emplace_back(glm::vec3{ leftBot.x, leftBot.y, leftBot.z }, fColor, bottom);
	indices.insert(indices.end(), { 20, 21, 22, 22, 23, 20 });

	return std::make_pair(indices, data);
}

std::pair<std::vector<uint32_t>, std::vector<real::PosTexNorm>> real::MeshFactory::CreateCubeMap(const glm::vec3& pos, float size)
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