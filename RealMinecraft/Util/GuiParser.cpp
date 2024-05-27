#include "GuiParser.h"

#include <glm/vec4.hpp>
#include <fstream>

#include "RealEngine.h"
#include "Content/ContentManager.h"
#include "Content/Texture2D.h"
#include "Util/GameUtils.h"

std::pair<std::vector<PosTex>, std::vector<uint32_t>> GuiParser::GetGuiElement(const std::string& name, int indexOffset)
{
    if (m_GuiElements.contains(name))
    {
        std::vector indices({ 0u + indexOffset,1u + indexOffset,2u + indexOffset,2u + indexOffset,3u + indexOffset,0u + indexOffset });
        std::vector vertices(m_GuiElements.at(name).vertices.begin(), m_GuiElements.at(name).vertices.end());

        return{ vertices,indices };
    }

    return {};
}

std::pair<std::vector<PosTex>, std::vector<uint32_t>> GuiParser::GetGui()
{
    std::vector<PosTex> vertices{};
    std::vector<uint32_t> indices{};

    int indexOffset = 0;
	for (const auto& element : m_GuiElements | std::views::values)
	{
        indices.insert(indices.end(), { 0u + indexOffset,1u + indexOffset,2u + indexOffset,2u + indexOffset,3u + indexOffset,0u + indexOffset });
        vertices.insert(vertices.end(), element.vertices.begin(), element.vertices.end());

        indexOffset += 4;
	}

    return { vertices, indices };
}

GuiParser::GuiParser()
{
	const auto context = real::RealEngine::GetGameContext();
    m_AspectRatio = static_cast<float>(context.windowWidth) / static_cast<float>(context.windowHeight);
	ParseGui();
}

void GuiParser::ParseGui()
{
    auto context = real::RealEngine::GetGameContext();

    std::ifstream ifs(m_GuiFile);
    if (!ifs.is_open())
    {
        std::cerr << "Couldn't open file " << m_GuiFile << '\n';
	    return;
    }

    nlohmann::json j;
    ifs >> j;

    const auto texturePath = j["texture"].get<std::string>();
    const auto texture = real::ContentManager::GetInstance().LoadTexture(context, texturePath);
    m_AtlasHeight = texture->GetHeight();
    m_AtlasWidth = texture->GetWidth();

    for (const auto& e : j["elements"])
    {
	    GuiElement element;
        const auto name = e["name"].get<std::string>();
        element.uv = IntVectorToVec<glm::vec4, 4>(e["uv"].get<std::vector<int>>());
        element.position = CalculatePos(e["position"], element);
        element.vertices = CalculateVertices(element);
        m_GuiElements[name] = element;
    }
}

glm::vec2 GuiParser::CalculatePos(const nlohmann::basic_json<>& json, const GuiElement& element)
{
    auto context = real::RealEngine::GetGameContext();
    glm::vec2 pos{ 0,0 };

    const auto& xComp = json["x"];
    auto from = xComp["from"].get<std::string>();
    auto pivot = xComp["pivot"].get<std::string>();
    float offset = xComp["dist"].get<float>() * 0.01f;
    const auto sizeX = (element.uv.z - element.uv.x) * m_PixelToScreen;

    if (pivot == "center")
        offset -= sizeX / 2.f;
    else if (pivot == "right")
        offset -= sizeX;

    if (from == "left")
        pos.x = -1 + offset;
    else if (from == "right")
        pos.x = 1 + offset;
    else
        pos.x += offset;

	const auto& yComp = json["y"];
    from = yComp["from"].get<std::string>();
    pivot = yComp["pivot"].get<std::string>();
	offset = yComp["dist"].get<float>() * 0.01f;
    const auto sizeY = (element.uv.w - element.uv.y) * m_PixelToScreen * m_AspectRatio;

    if (pivot == "center")
        offset -= sizeY / 2.f;
    else if (pivot == "bottom")
        offset -= sizeY;

    if (from == "top")
        pos.y = -1 + offset;
    else if (from == "bottom")
        pos.y = 1 + offset;
    else
        pos.y += offset;

    //return { 0,0 };
    return pos;
}

std::array<PosTex, 4> GuiParser::CalculateVertices(const GuiElement& element) const
{
    std::array<PosTex, 4> vertices{};

    const auto sizeX = (element.uv.z - element.uv.x) * m_PixelToScreen;
    const auto sizeY = (element.uv.w - element.uv.y) * m_PixelToScreen * m_AspectRatio;

    glm::vec2 leftTop = element.position;
    //leftTop.y *= m_AspectRatio;
    glm::vec2 rightBottom = element.position + glm::vec2(sizeX, sizeY);
    //rightBottom.y *= m_AspectRatio;

    std::vector<glm::vec2> positions;
    positions.emplace_back(glm::vec2{ rightBottom.x, leftTop.y });
    positions.emplace_back(glm::vec2{ leftTop.x, leftTop.y });
    positions.emplace_back(glm::vec2{ leftTop.x, rightBottom.y });
    positions.emplace_back(glm::vec2{ rightBottom.x, rightBottom.y });

    const auto texCoords = std::vector({
	    glm::vec2{element.uv.x, element.uv.w}, glm::vec2{element.uv.z, element.uv.w},
	    glm::vec2{element.uv.z, element.uv.y}, glm::vec2{element.uv.x, element.uv.y}
    });

	for (int i = 0; i < vertices.size(); ++i)
    {
        vertices[i].pos = glm::vec3(positions[i], 0.f);
        vertices[i].texCoord = texCoords[i];
        vertices[i].texCoord.x /= m_AtlasWidth;
        vertices[i].texCoord.y /= m_AtlasHeight;
    }

    return vertices;
}
