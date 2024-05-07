#include "SceneManager.h"

#include "GameTime.h"
#include "InputManager.h"
#include "Scene.h"

real::SceneManager::~SceneManager() = default;


void real::SceneManager::CreateScene(Scene* pScene)
{
	if (FindSceneWithName(pScene->GetName()) != nullptr)
	{
		Logger::LogError({ "Already a scene created with this name ({})" }, pScene->GetName());
		throw std::runtime_error("Already a scene created with this name (" + pScene->GetName() + ')');
	}

	m_ScenePtrs.push_back(std::unique_ptr<Scene>(pScene));

	//if (m_ScenePtrs.size() == 1)
	//{
	//	m_pActiveScene = m_ScenePtrs.back().get();
	//	InputManager::GetInstance().SetInputMapActive(pScene->GetDefaultInputMap());
	//}
}

void real::SceneManager::SetSceneActive(Scene& scene, float timer)
{
	if (&scene == m_pActiveScene)
	{
		Logger::LogWarning({ "Scene with name, {}, is already set as active" }, scene.GetName());
		return;
	}

	m_LoadTimer = timer;
	m_pSceneToLoad = &scene;
}

void real::SceneManager::SetSceneActive(const std::string& name, float timer)
{
	const auto scene = FindSceneWithName(name);

	if (scene == nullptr)
	{
		Logger::LogError({ "No scene found with name " }, name);
		throw std::runtime_error("No scene found with name " + name);
	}

	SetSceneActive(*scene, timer);
}

void real::SceneManager::FixedUpdate()
{
	m_pActiveScene->FixedUpdate();
}

void real::SceneManager::Update()
{
	if (m_pSceneToLoad != nullptr)
	{
		m_LoadTimer -= GameTime::GetInstance().GetElapsed();
		if (m_LoadTimer <= 0)
		{
			LoadScene();
		}
	}

	m_pActiveScene->Update();
}

void real::SceneManager::Render() const
{
	m_pActiveScene->Render();
}

void real::SceneManager::OnGui()
{
	m_pActiveScene->OnGui();
}

void real::SceneManager::Destroy()
{
	for (const auto& scene : m_ScenePtrs)
	{
		scene->Destroy();
		scene->Update();
	}
}

void real::SceneManager::LoadScene()
{
	if (m_pActiveScene != nullptr)
	{
		exitScene.Notify(SceneEvents::exit, m_pActiveScene);

		m_pActiveScene->Destroy();
		// One last update to actually remove the objects
		m_pActiveScene->Update();
	}

	m_pActiveScene = m_pSceneToLoad;
	m_pSceneToLoad = nullptr;

	if (m_pActiveScene->GetDefaultInputMap().empty() == false)
		InputManager::GetInstance().SetInputMapActive(m_pActiveScene->GetDefaultInputMap());

	m_pActiveScene->Load();
	m_pActiveScene->FirstFrame();

	loadScene.Notify(SceneEvents::load, m_pActiveScene);

	//InputManager::GetInstance().ReloadCommands();
}

real::Scene* real::SceneManager::FindSceneWithName(const std::string& name)
{
	const auto it = std::ranges::find_if(m_ScenePtrs, [&](const std::unique_ptr<Scene>& s) -> Scene*
		{
			if (s->GetName() == name)
				return s.get();

			return nullptr;
		});

	if (it == m_ScenePtrs.end())
		return nullptr;

	return (*it).get();
}

//real::Scene& real::SceneManager::AddScene(std::string name)
//{
//	m_CreatingScene = name;
//	auto pScene = std::make_unique<Scene>(name);
//	m_ScenePtrs.emplace_back(std::move(pScene));
//	return *m_ScenePtrs.back();
//}

//bool real::SceneManager::HasScene(const std::string& name)
//{
//	if (m_CreatingScene == name)
//		return true;
//
//	const bool found = std::ranges::any_of(m_ScenePtrs, [&](const std::unique_ptr<Scene>& s)
//		{
//			return s->GetName() == name;
//		});
//
//	return found;
//}
