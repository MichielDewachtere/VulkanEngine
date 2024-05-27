#include "GUI.h"
#include "Util/GuiParser.h"

Gui::Gui(real::GameObject* pOwner)
	: Component(pOwner)
{
}

void Gui::Start()
{
	m_pMeshComponent = GetOwner()->GetComponent<real::MeshIndexed<PosTex, WorldMatrix>>();

	std::vector<PosTex> verticesCorrect;
	{
		auto context = real::RealEngine::GetGameContext();
		float aspectRatio = static_cast<float>(context.windowWidth) / static_cast<float>(context.windowHeight);

		const glm::vec2 leftTop = { -0.05f,-0.05f * aspectRatio };
		const glm::vec2 rightBottom = { 0.05f,0.05f * aspectRatio };

		std::vector<glm::vec2> positions;
		positions.emplace_back(glm::vec2{ rightBottom.x, leftTop.y });
		positions.emplace_back(glm::vec2{ leftTop.x, leftTop.y });
		positions.emplace_back(glm::vec2{ leftTop.x, rightBottom.y });
		positions.emplace_back(glm::vec2{ rightBottom.x, rightBottom.y });


		const auto texCoords = std::vector({
		glm::vec2{0, 1}, glm::vec2{1,1},
		glm::vec2{1,0}, glm::vec2{0,0}
			});


		for (size_t i = 0; i < positions.size(); ++i)
		{
			PosTex vertex;
			vertex.pos = glm::vec3(positions[i], 0);
			vertex.texCoord = texCoords[i];
			verticesCorrect.push_back(vertex);
		}

		const std::vector indices({ 0u,1u,2u,2u,3u,0u });
	}

	//{
	//	auto [vertices, indices] = GuiParser::GetInstance().GetGuiElement("crosshair", 0);
	//	m_pMeshComponent->AddVertices(vertices);
	//	m_pMeshComponent->AddIndices(indices);
	//}
	//{
	//	auto [vertices, indices] = GuiParser::GetInstance().GetGuiElement("hotbar", 4);
	//	m_pMeshComponent->AddVertices(vertices);
	//	m_pMeshComponent->AddIndices(indices);
	//}

	auto [vertices, indices] = GuiParser::GetInstance().GetGui();
	m_pMeshComponent->AddVertices(vertices);
	m_pMeshComponent->AddIndices(indices);
}
