#ifndef SCENE_H
#define SCENE_H

#include <stdint.h>

#include "GameObject.h"

namespace real
{
	class Scene
	{
	public:
		explicit Scene(std::string name, std::string inputMapName);
		virtual ~Scene();

		Scene(const Scene& other) = delete;
		Scene& operator=(const Scene& other) = delete;
		Scene(Scene&& other) = delete;
		Scene& operator=(Scene&& other) = delete;

		virtual void Load() = 0;

		void FixedUpdate();
		void Update();
		void Render() const;
		void OnGui();

		const std::string& GetName() const { return m_Name; }

		void SetDefaultInputMap(std::string inputMap) { m_InputMapName = std::move(inputMap); }
		std::string GetDefaultInputMap() const { return m_InputMapName; }

		GameObject& CreateGameObject(std::string tag = "");
		void AddGameObject(GameObject* pGo);
		void AddGameObject(std::unique_ptr<GameObject> pGo);
		GameObject* GetGameObject(uint32_t id) const;
		std::unique_ptr<GameObject> GetUniqueGameObject(GameObject* pGo);
		std::unique_ptr<GameObject> GetUniqueGameObject(uint32_t id);

		std::vector<GameObject*> FindGameObjectsWithTag(std::string tag) const;

		void Remove(GameObject* pGo);
		void RemoveAll();
		void Destroy();

		void FirstFrame() { m_IsFirstFrame = true; }

	private:
		friend class GameObject;

		bool m_IsFirstFrame{ true }, m_DeleteGameObject{ false };
		std::string m_Name, m_InputMapName;
		std::vector<std::unique_ptr<GameObject>> m_GameObjects{};

		static unsigned int m_IdCounter; 
	};

}

#endif // SCENE_H