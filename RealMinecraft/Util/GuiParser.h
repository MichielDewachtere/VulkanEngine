#ifndef GUIPARSER_H
#define GUIPARSER_H

#include <map>
#include <string>
#include <glm/vec2.hpp>
#include <nlohmann/json.hpp>

#include <real_core/Singleton.h>

#include "GameStructs.h"

class GuiParser final : public real::Singleton<GuiParser>
{
public:
	virtual ~GuiParser() override = default;

	GuiParser(const GuiParser& other) = delete;
	GuiParser& operator=(const GuiParser& rhs) = delete;
	GuiParser(GuiParser&& other) = delete;
	GuiParser& operator=(GuiParser&& rhs) = delete;

	std::pair<std::vector<PosTex>, std::vector<uint32_t>> GetGuiElement(const std::string& name, int indexOffset);
	std::pair<std::vector<PosTex>, std::vector<uint32_t>> GetGui();

private:
	friend class Singleton<GuiParser>;
	explicit GuiParser();

	std::string m_GuiFile{ "Resources\\GUI\\gui.json" };
	std::map<std::string, GuiElement> m_GuiElements;
	int m_AtlasWidth{}, m_AtlasHeight{};
	float m_PixelToScreen{ 0.006667f }, m_AspectRatio{};

	void ParseGui();
	glm::vec2 CalculatePos(const nlohmann::basic_json<>& json, const GuiElement& element);
	std::array<PosTex, 4> CalculateVertices(const GuiElement& element) const;
};

#endif // GUIPARSER_H