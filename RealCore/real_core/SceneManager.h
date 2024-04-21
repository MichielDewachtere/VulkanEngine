#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include "Singleton.h"
#include "Scene.h"

namespace real
{
	enum  class SceneEvents : char
	{
		exit = 0,
		load = 1
	};

	// TODO: Add scene switch events
	class SceneManager final : public Singleton<SceneManager>
	{
	public:
		~SceneManager() override;
		SceneManager(const SceneManager& other) = delete;
		SceneManager& operator=(const SceneManager& other) = delete;
		SceneManager(SceneManager&& other) = delete;
		SceneManager& operator=(SceneManager&& other) = delete;

		//Scene& CreateScene(std::string name, const std::string& inputMap);
		void CreateScene(Scene* pScene);

		void SetSceneActive(Scene& scene, float timer = 0);
		void SetSceneActive(const std::string& name, float timer = 0);

		Scene& GetActiveScene() const { return *m_pActiveScene; }

		void FixedUpdate();
		void Update();
		void Render() const;
		void OnGui();

		void Destroy();

		real::Subject<SceneEvents, Scene*> exitScene;
		real::Subject<SceneEvents, Scene*> loadScene;

	private:
		friend class Singleton<SceneManager>;
		SceneManager() = default;

		std::vector<std::unique_ptr<Scene>> m_ScenePtrs;
		Scene* m_pActiveScene{ nullptr };
		Scene* m_pSceneToLoad{ nullptr };
		float m_LoadTimer{ 0 };

		void LoadScene();
		Scene* FindSceneWithName(const std::string& name);
	};
}

#endif // SCENEMANAGER_H