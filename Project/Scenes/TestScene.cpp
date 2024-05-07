#include "TestScene.h"

#include "RealEngine.h"
#include "Material/MaterialManager.h"
#include "Material/Pipelines/PosCol2DPipeline.h"
#include "Material/Pipelines/PosColNormPipeline.h"
#include "Material/Pipelines/PosTexNormPipeline.h"
#include "Mesh/Mesh.h"
#include "Mesh/MeshFactory.h"
#include "Mesh/MeshIndexed.h"
#include "Misc/CameraManager.h"

TestScene::TestScene(std::string name, std::string inputMap)
	: Scene(name, inputMap)
{
}

void TestScene::Load()
{
	const auto& materialManager = MaterialManager::GetInstance();
	const auto context = RealEngine::GetGameContext();

	{
		auto& triangle = CreateGameObject();
		triangle.GetTransform()->SetLocalPosition({ 0.5f,0 });

		const std::vector triangleVertices{
			PosCol2D{ glm::vec2{0.5f + 0.0f, -0.25f},	glm::vec3{1.0f, 0.0f, 0.0f} },
			PosCol2D{ glm::vec2{0.5f + 0.25f,  0.25f},glm::vec3{0.0f, 1.0f, 0.0f} },
			PosCol2D{ glm::vec2{0.5f + -0.25f, 0.25f},glm::vec3{0.0f, 0.0f, 1.0f} },
		};
		MeshInfo info;
		info.vertexCapacity = static_cast<uint32_t>(triangleVertices.size());

		const auto mesh = triangle.AddComponent<Mesh<PosCol2D>>(info);
		materialManager.GetMaterial<PosCol2DPipeline, PosCol2D>()->BindMesh(context, mesh);
		mesh->AddVertices(triangleVertices);
		mesh->Init(context);
	}
	{
		auto& rectangle = CreateGameObject();
		rectangle.GetTransform()->SetLocalPosition({ 0.5f,0 });

		const std::vector vertices = {
			PosCol2D{ glm::vec2{-0.5 + -0.25, -0.25},	glm::vec3{1,0,0} },
			PosCol2D{ glm::vec2{-0.5 + 0.25,  -0.25},	glm::vec3{0,1,0} },
			PosCol2D{ glm::vec2{-0.5 + 0.25,   0.25},	glm::vec3{0,0,1} },
			PosCol2D{ glm::vec2{-0.5 + -0.25,  0.25},	glm::vec3{1,1,1} },
		};
		const std::vector<uint32_t> indices =
		{
			0, 1, 2, 2, 3, 0
		};

		MeshInfo info;
		info.indexCapacity = static_cast<uint32_t>(indices.size());
		info.vertexCapacity = static_cast<uint32_t>(vertices.size());

		const auto mesh = rectangle.AddComponent<MeshIndexed<PosCol2D>>(info);
		materialManager.GetMaterial<PosCol2DPipeline, PosCol2D>()->BindMesh(context, mesh);
		mesh->AddVertices(vertices);
		mesh->AddIndices(indices);
		mesh->Init(context);
	}

	{
		auto& coloredCube = CreateGameObject();
		coloredCube.GetTransform()->SetLocalPosition({ -1,0,0 });

		auto [indices, vertices] = MeshFactory::CreateCube({ 0,0,0 }, 1);
		MeshInfo info;
		info.indexCapacity = static_cast<uint32_t>(indices.size());
		info.vertexCapacity = static_cast<uint32_t>(vertices.size());
		info.usesUbo = true;

		const auto mesh = coloredCube.AddComponent<MeshIndexed<PosColNorm>>(info);
		materialManager.GetMaterial<PosColNormPipeline, PosColNorm>()->BindMesh(context, mesh);
		mesh->AddVertices(vertices);
		mesh->AddIndices(indices);
		mesh->Init(context);
	}

	{
		auto& coloredPyramid = CreateGameObject();
		coloredPyramid.GetTransform()->SetLocalPosition({ -3,0,0 });

		auto [indices, vertices] = MeshFactory::CreatePyramid({ 0,0,0 }, 1, 1);
		MeshInfo info;
		info.indexCapacity = static_cast<uint32_t>(indices.size());
		info.vertexCapacity = static_cast<uint32_t>(vertices.size());
		info.usesUbo = true;

		const auto mesh = coloredPyramid.AddComponent<MeshIndexed<PosColNorm>>(info);
		materialManager.GetMaterial<PosColNormPipeline, PosColNorm>()->BindMesh(context, mesh);
		mesh->AddVertices(vertices);
		mesh->AddIndices(indices);
		mesh->Init(context);
	}

	{
		auto& texturedCube = CreateGameObject();
		texturedCube.GetTransform()->SetLocalPosition({ 1,0,0 });

		auto [indices, vertices] = MeshFactory::CreateCubeMap({ 0,0,0 }, 1);
		MeshInfo info;
		info.indexCapacity = static_cast<uint32_t>(indices.size());
		info.vertexCapacity = static_cast<uint32_t>(vertices.size());
		info.usesUbo = true;
		info.texture = ContentManager::GetInstance().LoadTexture(context, "Resources/textures/grass_side.png");

		const auto mesh = texturedCube.AddComponent<MeshIndexed<PosTexNorm>>(info);
		materialManager.GetMaterial<PosTexNormPipeline, PosTexNorm>()->BindMesh(context, mesh);
		mesh->AddVertices(vertices);
		mesh->AddIndices(indices);
		mesh->Init(context);
	}

	{
		auto& model = CreateGameObject();
		model.GetTransform()->SetLocalPosition({ 3,0,0 });
		model.GetTransform()->SetPitch(270);
		model.GetTransform()->SetRoll(270);

		const auto loadedModel = ContentManager::GetInstance().LoadModel("Resources/Models/viking_room.obj", { 0,0,0 });
		const auto indices = loadedModel->GetIndices();
		const auto vertices = loadedModel->GetVertices();
		MeshInfo info;
		info.indexCapacity = static_cast<uint32_t>(indices.size());
		info.vertexCapacity = static_cast<uint32_t>(vertices.size());
		info.usesUbo = true;
		info.texture = ContentManager::GetInstance().LoadTexture(context, "Resources/textures/viking_room.png");

		const auto mesh = model.AddComponent<MeshIndexed<PosTexNorm>>(info);
		materialManager.GetMaterial<PosTexNormPipeline, PosTexNorm>()->BindMesh(context, mesh);
		mesh->AddVertices(vertices);
		mesh->AddIndices(indices);
		mesh->Init(context);
	}

	{
		real::TransformInfo info;
		info.position = { 0,0,2 };
		auto& camera = CreateGameObject(info);
		auto comp = camera.AddComponent<Camera>();
		CameraManager::GetInstance().AddCamera(comp, true);
	}
}