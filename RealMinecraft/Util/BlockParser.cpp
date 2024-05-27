#include "BlockParser.h"

#include <fstream>

#include "GameUtils.h"

BlockParser::vertices_and_indices BlockParser::GetFaceData(EDirection dir, EBlock block, glm::vec3 pos, int indexOffset)
{
    if (block == EBlock::air)
        return {};

    if (m_Blocks.contains(block) == false)
    {
        ParseBlock(block);
    }

    std::vector<real::PosTexNorm> vertices;
    std::vector<uint32_t> indices;
    int counter = 0;
    for (const auto& element : m_Blocks.at(block).elements)
    {
        if (element.faces.contains(dir) == false)
            continue;

        auto v = element.faces.at(dir).vertices;
        std::ranges::for_each(v, [pos](real::PosTexNorm& vertex)
            {
                vertex.pos += pos;
            });

        vertices.insert(vertices.end(), v.begin(), v.end());
        indices.insert(indices.end(), {
	                       0u + indexOffset + counter, 1u + indexOffset + counter, 2u + indexOffset + counter,
	                       2u + indexOffset + counter, 3u + indexOffset + counter, 0u + indexOffset + counter
                       });
        counter += 6;
    }

    return { vertices, indices };
}

BlockParser::vertices_and_indices BlockParser::GetBlockData(EBlock block, glm::vec3 pos, int indexOffset)
{
    if (block == EBlock::air)
        return {};

    if (m_Blocks.contains(block) == false)
    {
        ParseBlock(block);
    }

    //const auto model = m_Blocks.at(block);
    //if (model.parent == EBlockType::cross)
    //    return {};

    //std::vector<glm::vec3> vertices;
    //if (model.parent == EBlockType::cross)
    //    vertices = GetVertexPositionsCross(model.elements);
    //else
    //{
    //    for (int i = 0; i < static_cast<int>(EDirection::amountOfDirections); ++i)
    //    {
    //        auto v = GetVertexPositions(model.elements.front(), static_cast<EDirection>(i));
    //        vertices.insert(vertices.end(), v.begin(), v.end());
    //    }
    //}

    //std::vector<real::PosTexNorm> data;
    //data.resize(vertices.size(), {});
    //if (vertices.size() % 4 != 0)
    //    return {};
    // 
    //for (size_t i{ 0 }; i < data.size(); ++i)
    //{
    //    data[i].pos = vertices[i] + pos;
    //    data[i].normal = direction_to_normal.at(dir);
    //    const int textureIndex = texture_index_map.at({ model.parent, dir });
    //    data[i].texCoord = GetTexCoord(model.textures.at(textureIndex), static_cast<int>(i), dir, model);
    //}



    //return data;
    return {};
}

bool BlockParser::IsTransparent(EBlock block)
{
    if (block == EBlock::air)
        return false;

    if (block == EBlock::water)
        return true;

    if (m_Blocks.contains(block) == false)
        ParseBlock(block);

    return m_Blocks.at(block).transparent;
}

bool BlockParser::IsFullFace(EBlock block, EDirection dir)
{
    if (m_Blocks.contains(block) == false)
        ParseBlock(block);

    auto isFullFace = false;
	for (const auto& element : m_Blocks.at(block).elements)
	{
		if (element.faces.contains(dir) == false)
            continue;

        if (element.faces.at(dir).isFull)
        {
            isFullFace = true;
	        continue;
        }

        return false;
	}

    return isFullFace;
}

bool BlockParser::IsCrossBlock(EBlock block)
{
    if (m_Blocks.contains(block) == false)
        ParseBlock(block);

    return m_Blocks.at(block).parent == EBlockType::cross;
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

    switch (dir)
    {
    case EDirection::north:
        vertices.emplace_back(glm::vec3{ rightTop.x, leftBot.y, rightTop.z });
        vertices.emplace_back(glm::vec3{ leftBot.x, leftBot.y, rightTop.z });
        vertices.emplace_back(glm::vec3{ leftBot.x, rightTop.y, rightTop.z });
        vertices.emplace_back(glm::vec3{ rightTop.x, rightTop.y, rightTop.z });
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

std::vector<glm::vec3> BlockParser::GetVertexPositionsCross(const BlockElement& e)
{
    std::vector<glm::vec3> vertices;

    constexpr float blockSize = 16;
    const glm::vec3 leftBot = e.from / blockSize;
    const glm::vec3 rightTop = e.to / blockSize;

    vertices.emplace_back(glm::vec3{ leftBot.x, leftBot.y, leftBot.z });
    vertices.emplace_back(glm::vec3{ rightTop.x, leftBot.y, rightTop.z });
    vertices.emplace_back(glm::vec3{ rightTop.x, rightTop.y, rightTop.z });
    vertices.emplace_back(glm::vec3{ leftBot.x, rightTop.y, leftBot.z });

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

        if (j.contains("transparent"))
            model.transparent = j["transparent"].get<bool>();

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

    CalculateVertexData(model);

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

        if (json.contains("rotation"))
        {
            RotateElement(element, json["rotation"]);
        }

        if (json.contains("faces"))
        {
            FillFaces(element, json["faces"]);
        }

        model.elements.push_back(element);
    }
    else
    {
        bool rotate = false;
        if (model.elements.at(i).to == glm::vec3{ -1 } && json.contains("to"))
        {
	        model.elements.at(i).to = IntVectorToVec<glm::vec3, 3>(json["to"].get<std::vector<int>>());
            rotate = true;
        }

    	if (model.elements.at(i).from == glm::vec3{ -1 } && json.contains("from"))
    	{
    		model.elements.at(i).from = IntVectorToVec<glm::vec3, 3>(json["from"].get<std::vector<int>>());
            rotate = true;
    	}

    	if (rotate && json.contains("rotate"))
        {
            RotateElement(model.elements.at(i), json["rotate"]);
        }
    }
}

void BlockParser::FillParent(BlockModel& model, const std::string& parent)
{
    if (model.parent == EBlockType::block)
    {
        const auto pos = parent.find('/');
        if (pos == std::string::npos)
        {
	        model.parent = name_to_block_type.at(parent);
        }
        else
        {
            const auto type = parent.substr(pos + 1);
	        model.parent = name_to_block_type.at(type);
        }
    }
}

void BlockParser::RotateElement(BlockElement& element, const nlohmann::basic_json<>& json) const
{
    auto origin = IntVectorToVec<glm::vec3, 3>(json["origin"].get<std::vector<int>>());
    const char axis = json["axis"].get<std::string>().front();
    const float angle = glm::radians(json["angle"].get<float>());

    glm::mat4 rotationMatrix(1.0f);
    if (axis == 'x')
        rotationMatrix = glm::rotate(rotationMatrix, angle, glm::vec3(1.0f, 0.0f, 0.0f));
    else if (axis == 'y')
        rotationMatrix = glm::rotate(rotationMatrix, angle, glm::vec3(0.0f, 1.0f, 0.0f));
    else if (axis == 'z')
        rotationMatrix = glm::rotate(rotationMatrix, angle, glm::vec3(0.0f, 0.0f, 1.0f));

    // Invert z-axis
    origin.z *= -1;
    element.from.z *= -1;
    element.to.z *= -1;

    // Translate the points to the origin
    const auto fromTranslated = glm::vec4(element.from - origin, 1.0f);
    const auto toTranslated = glm::vec4(element.to - origin, 1.0f);

    // Apply rotation
    const glm::vec4 fromRotated = rotationMatrix * fromTranslated;
    const glm::vec4 toRotated = rotationMatrix * toTranslated;

    // Translate the points back
    element.from = glm::vec3(fromRotated) + origin;
    element.to = glm::vec3(toRotated) + origin;
}

void BlockParser::FillFaces(BlockElement& element, const nlohmann::basic_json<>& json) const
{
    for (int i = 0; i < static_cast<int>(EDirection::amountOfDirections); ++i)
    {
        const auto dir = static_cast<EDirection>(i);
        const auto& dirName = direction_to_name.at(dir);
		if (json.contains(dirName) == false) continue;

        BlockFace face;
        if (element.faces.contains(dir))
            face = element.faces.at(dir);

        if (json.contains("uv") && face.uv == glm::vec4{ 0, 0, 16, 16 })
            face.uv = IntVectorToVec<glm::vec4, 4>(json["uv"].get<std::vector<int>>());

        if (json.contains("full_face") && face.isFull == false)
            face.isFull = json["full_face"].get<bool>();

        element.faces[dir] = face;
    }
}

void BlockParser::CalculateVertexData(BlockModel& model) const
{
	for (auto& element : model.elements)
	{
        for (auto& [direction, face] : element.faces)
        {
            std::array<real::PosTexNorm, 4> vertices{};

            std::vector<glm::vec3> v;
            if (model.parent == EBlockType::cross)
                v = GetVertexPositionsCross(element);
            else
				v = GetVertexPositions(element, direction);

            for (int i = 0; i < vertices.size(); ++i)
            {
                vertices[i].pos = v[i];
                vertices[i].normal = direction_to_normal.at(direction);
                const int textureIndex = texture_index_map.at({ model.parent, direction });
                vertices[i].texCoord = GetTexCoord(model.textures.at(textureIndex), static_cast<int>(i), direction, model);
            }

            face.vertices = vertices;
        }
	}
}
