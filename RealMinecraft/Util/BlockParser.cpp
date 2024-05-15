#include "BlockParser.h"

#include <fstream>

#include "GameUtils.h"

std::vector<real::PosTexNorm> BlockParser::GetFaceData(EDirection dir, const EBlock block, glm::vec3 pos)
{
    if (block == EBlock::air)
        return {};

    if (m_Blocks.contains(block) == false)
    {
        ParseBlock(block);
    }

	const auto model = m_Blocks.at(block);

    //TODO: Only supports cubes for now
	const auto vertices = GetVertexPositions(model.elements.front(), dir);
	std::vector<real::PosTexNorm> data;
	data.resize(4, {});

	for (size_t i{0}; i < data.size(); ++i)
	{
        data[i].pos = vertices[i] + pos;
        data[i].normal = direction_to_normal.at(dir);
        const int textureIndex = texture_index_map.at({ model.parent, dir });
        data[i].texCoord = GetTexCoord(model.textures.at(textureIndex), static_cast<int>(i), dir, model);
	}

	return data;
}

BlockParser::BlockParser()
{
    const float sizeX = static_cast<float>(m_AmountOfTexX) * static_cast<float>(m_TextureSize);
    const float sizeY = static_cast<float>(m_AmountOfTexY) * static_cast<float>(m_TextureSize);
    m_NormalTextureWidth = sizeX / (sizeX * static_cast<float>(m_TextureSize));
    m_NormalTextureHeight = sizeY / (sizeY * static_cast<float>(m_TextureSize));
}

std::vector<glm::vec3> BlockParser::GetVertexPositions(const BlockElement& e, const EDirection dir)
{
    constexpr float blockSize = 16;

    const glm::vec3 leftBot = e.from / blockSize;
    glm::vec3 rightTop = e.to / blockSize;
    rightTop.z *= -1;

    std::vector<glm::vec3> vertices;

    switch (dir) {
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

glm::vec2 BlockParser::GetTexCoord(int atlasId, int vertexId, EDirection dir, BlockModel model) const
{
    const float posX = static_cast<float>(atlasId * m_TextureSize % m_AtlasSizeX) / static_cast<float>(m_AtlasSizeX);
    const float posY = static_cast<float>(atlasId / m_AmountOfTexY) / static_cast<float>(m_AmountOfTexY);

    const auto uv = model.elements.front().faces[dir].uv;
	const auto v = std::vector({ glm::vec2{ uv.x,uv.w }, glm::vec2{ uv.z,uv.w }, glm::vec2{ uv.z,uv.y }, glm::vec2{ uv.x,uv.y } });
    auto texCoord = v[vertexId];
    texCoord /= glm::vec2{ m_AmountOfTexX, m_AmountOfTexY };
    texCoord /= m_TextureSize;

    return { posX + texCoord.x, posY + texCoord.y };
}

void BlockParser::ParseBlock(EBlock block)
{
    using json = nlohmann::json;

    const std::string path{"resources/models/"};
    const std::string extension{ ".json" };

    std::string file = path + "block/" + block_to_name.at(block) + extension;
    std::string parent{"empty"};
    BlockModel model{};

    while (parent != "block")
    {
        // Open the JSON file
        std::ifstream ifs(file);
        if (!ifs.is_open())
            return;

        // Parse the JSON
        json j;
        ifs >> j;

        // Extract data from the parsed JSON
        parent = j["parent"];

        FillParent(model, parent);

    	file.clear();
        file.append(path);
        file.append(parent);
        file.append(extension);

        if (j.contains("textures"))
            model.textures = j["textures"].get<std::vector<int>>();

        if (j.contains("elements"))
        {
	        auto elements = j["elements"];
        	for (size_t i = 0; i < elements.size(); ++i)
        	{
        		nlohmann::basic_json<> element = elements.at(i);
        		FillElement(model, i, element);
        	}
        }
    }

    m_Blocks[block] = model;
}

void BlockParser::FillElement(BlockModel& model, size_t i, const nlohmann::basic_json<>& json) const
{
    if (model.elements.size() <= i)
    {
        BlockElement element{};
        if (json.contains("from"))
            element.from = IntVectorToVec<glm::vec3, 3>(json["from"].get<std::vector<int>>());
        if (json.contains("to"))
        element.to = IntVectorToVec<glm::vec3, 3>(json["to"].get<std::vector<int>>());

        model.elements.push_back(element);
    }
    else
    {
        if (model.elements.at(i).to == glm::vec3{ -1 } && json.contains("to"))
            model.elements.at(i).to = IntVectorToVec<glm::vec3, 3>(json["to"].get<std::vector<int>>());

    	if (model.elements.at(i).from == glm::vec3{ -1 } && json.contains("from"))
            model.elements.at(i).from = IntVectorToVec<glm::vec3, 3>(json["from"].get<std::vector<int>>());
    }
}

void BlockParser::FillParent(BlockModel& model, const std::string& parent)
{
    if (model.parent == EBlockType::block)
    {
        const auto pos = parent.find('/');
        if (pos == std::string::npos)
            model.parent = name_to_block_type.at(parent);
        else
        {
            const auto type = parent.substr(pos + 1);
	        model.parent = name_to_block_type.at(type);
        }
    }
}
