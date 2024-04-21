#include "Scene.h"

#include <algorithm>

#include "imgui.h"

using namespace real;

unsigned int Scene::m_IdCounter = 0;

Scene::Scene(std::string name, std::string inputMapName)
	: m_Name(std::move(name))
	, m_InputMapName(std::move(inputMapName))
{
}

Scene::~Scene() = default;

GameObject& Scene::CreateGameObject(std::string tag)
{
	auto pGameObject = std::make_unique<GameObject>(this, std::move(tag));
	m_GameObjects.emplace_back(std::move(pGameObject));
	return *m_GameObjects.back();
}

void Scene::AddGameObject(GameObject* pGo)
{
	AddGameObject(std::unique_ptr<GameObject>(pGo));
}

void Scene::AddGameObject(std::unique_ptr<GameObject> pGo)
{
	pGo->SetScene(this);
	m_GameObjects.emplace_back(std::move(pGo));
}

GameObject* Scene::GetGameObject(uint32_t id) const
{
	for (const auto& go : m_GameObjects)
	{
		if (go->GetId() == id)
			return go.get();

		if (const auto child = go->GetChild(id);
			child != nullptr)
			return child;
	}

	return nullptr;
}

std::unique_ptr<GameObject> Scene::GetUniqueGameObject(GameObject* pGo)
{
	return GetUniqueGameObject(pGo->GetId());
}

std::unique_ptr<GameObject> Scene::GetUniqueGameObject(uint32_t id)
{
	for (auto it = m_GameObjects.begin(); it < m_GameObjects.end(); ++it)
	{
		if ((*it)->GetId() != id)
			continue;

		auto pUnique{ std::move(*it) };
		m_GameObjects.erase(it);

		return pUnique;
	}

	return nullptr;
}

std::vector<GameObject*> Scene::FindGameObjectsWithTag(std::string tag) const
{
	std::vector<GameObject*> v;

	if (m_GameObjects.empty() == false)
	{
		for (const auto& go : m_GameObjects)
		{
			if (go->IsMarkedForDestroy())
				continue;

			if (go->GetTag() == tag)
				v.push_back(go.get());

			auto sub = go->GetGameObjectsWithTag(tag);
			v.insert(v.end(), sub.begin(), sub.end());
		}
	}

	return v;

}

void Scene::Remove(GameObject* pGo)
{
	pGo->Destroy();
	m_DeleteGameObject = true;
}

void Scene::RemoveAll()
{
	Destroy();
	//m_GameObjects.clear();
}

void Scene::Destroy()
{
	for (const auto& object : m_GameObjects)
	{
		object->Destroy();
	}
	m_DeleteGameObject = true;
}

void Scene::FixedUpdate()
{
	std::ranges::for_each(m_GameObjects, [](const auto& go)
		{
			go->FixedUpdate();
		});
}

void Scene::Update()
{
	if (m_IsFirstFrame)
	{
		std::ranges::for_each(m_GameObjects, [](const auto& go)
			{
				go->Start();
			});
		m_IsFirstFrame = false;
	}

	std::ranges::for_each(m_GameObjects, [](const auto& go)
		{
			go->Update();
		});

	std::ranges::for_each(m_GameObjects, [](const auto& go)
		{
			go->LateUpdate();
		});

	if (m_DeleteGameObject)
	{
		for (auto it = m_GameObjects.begin(); it != m_GameObjects.end();)
		{
			if ((*it)->IsMarkedForDestroy())
			{
				it = m_GameObjects.erase(it);
			}
			else
				++it;
		}

		m_DeleteGameObject = false;
	}
}

void Scene::Render() const
{
	std::ranges::for_each(m_GameObjects, [](const auto& go)
		{
			go->Render();
		});

#ifdef _DEBUG
	std::ranges::for_each(m_GameObjects, [](const auto& go)
		{
			go->DebugRender();
		});
#endif
}

void Scene::OnGui()
{
	ImGui::Begin("Scene Graph - WIP");
	//ImGui::SetWindowSize("Scene Graph", { 150,12 });
	std::ranges::for_each(m_GameObjects, [](const auto& go)
		{
			go->OnGui();
		});
	ImGui::End();
}
