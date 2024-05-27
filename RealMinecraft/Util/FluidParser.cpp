#include "FluidParser.h"

#include "BlockParser.h"

std::vector<real::PosTexNorm> FluidParser::GetFaceData(EDirection dir, glm::vec3 pos, bool waterAbove) const
{
    const auto vertices = GetVertexPositions(dir, waterAbove);
    std::vector<real::PosTexNorm> data;
    data.resize(4, {});

	for (size_t i{ 0 }; i < data.size(); ++i)
    {
        data[i].pos = vertices[i] + pos;
        data[i].normal = BlockParser::direction_to_normal.at(dir);
        data[i].texCoord = GetTexCoord(static_cast<int>(i));
    }

	return data;
}

std::vector<glm::vec3> FluidParser::GetVertexPositions(EDirection dir, bool waterAbove)
{
    constexpr float blockSize = 16;

    constexpr auto leftBot = glm::vec3{ 0,0,0 };
    const auto rightTop = glm::vec3{ 16,waterAbove ? 16 : 15,-16 } / blockSize;

    std::vector<glm::vec3> vertices;

    switch (dir)
    {
    case EDirection::north:
        vertices.emplace_back(glm::vec3{ leftBot.x, rightTop.y, rightTop.z });
        vertices.emplace_back(glm::vec3{ rightTop.x, rightTop.y, rightTop.z });
        vertices.emplace_back(glm::vec3{ rightTop.x, leftBot.y, rightTop.z });
        vertices.emplace_back(glm::vec3{ leftBot.x, leftBot.y, rightTop.z });
        break;
    case EDirection::east:
        vertices.emplace_back(glm::vec3{ rightTop.x, leftBot.y, leftBot.z });
        vertices.emplace_back(glm::vec3{ rightTop.x, leftBot.y, rightTop.z });
        vertices.emplace_back(glm::vec3{ rightTop.x, rightTop.y, rightTop.z });
        vertices.emplace_back(glm::vec3{ rightTop.x, rightTop.y, leftBot.z });
        break;
    case EDirection::south:
        vertices.emplace_back(glm::vec3{ leftBot.x, leftBot.y, leftBot.z });
        vertices.emplace_back(glm::vec3{ rightTop.x, leftBot.y, leftBot.z });
        vertices.emplace_back(glm::vec3{ rightTop.x, rightTop.y, leftBot.z });
        vertices.emplace_back(glm::vec3{ leftBot.x, rightTop.y, leftBot.z });
        break;
    case EDirection::west:
        vertices.emplace_back(glm::vec3{ leftBot.x, leftBot.y, rightTop.z });
        vertices.emplace_back(glm::vec3{ leftBot.x, leftBot.y, leftBot.z });
        vertices.emplace_back(glm::vec3{ leftBot.x, rightTop.y, leftBot.z });
        vertices.emplace_back(glm::vec3{ leftBot.x, rightTop.y, rightTop.z });
        break;
    case EDirection::up:
        vertices.emplace_back(glm::vec3{ leftBot.x, rightTop.y, leftBot.z });
        vertices.emplace_back(glm::vec3{ rightTop.x, rightTop.y, leftBot.z });
        vertices.emplace_back(glm::vec3{ rightTop.x, rightTop.y, rightTop.z });
        vertices.emplace_back(glm::vec3{ leftBot.x, rightTop.y, rightTop.z });
        break;
    case EDirection::down:
        vertices.emplace_back(glm::vec3{ leftBot.x, leftBot.y, rightTop.z });
        vertices.emplace_back(glm::vec3{ rightTop.x, leftBot.y, rightTop.z });
        vertices.emplace_back(glm::vec3{ rightTop.x, leftBot.y, leftBot.z });
        vertices.emplace_back(glm::vec3{ leftBot.x, leftBot.y, leftBot.z });
        break;
    }

    return vertices;
}

glm::vec2 FluidParser::GetTexCoord(int i) const
{
    const float textureHeight = m_TextureSize / m_SheetHeight;
    constexpr float textureWidth = 1.f;
    const glm::vec4 uv = { 0,0,textureWidth, textureHeight };

    const auto v = std::vector({ glm::vec2{ uv.x,uv.w }, glm::vec2{ uv.x,uv.y }, glm::vec2{ uv.z,uv.y }, glm::vec2{ uv.z,uv.w } });

    return v[i];
}
