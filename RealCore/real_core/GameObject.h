#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <stdint.h>
#include <vector>
#include <memory>
#include <string>
#include <algorithm>

#include "Transform.h"

namespace real
{
	class Scene;
	class Texture2D;

	enum class GameObjectEvent : char
	{
		destroyed = 0
	};

	class GameObject final
	{
	public:
		explicit GameObject(Scene * scene, std::string tag = "none");
		~GameObject() = default;

		GameObject(const GameObject& other) = delete;
		GameObject& operator=(const GameObject& other) = delete;
		GameObject(GameObject&& other) = delete;
		GameObject& operator=(GameObject&& other) = delete;

		GameObject& CreateGameObject(std::string tag = "");
		uint32_t GetId() const { return m_Id; }

		void Start();
		void FixedUpdate();
		void Update();
		void LateUpdate();
		void Render() const;
		void DebugRender() const;
		void OnGui();

		void Destroy();
		void Destroy(float time);
		bool IsMarkedForDestroy() const { return m_IsMarkedForDestroy; }
		void SetIsActive(bool isEnabled, bool applyToChildren);
		bool IsActive() const { return m_IsActive && m_IsMarkedForDestroy == false; }

		void SetScene(Scene* pScene);
		Scene& GetScene() const { return *m_pScene; }

		void SetTag(std::string tag) { m_Tag = std::move(tag); }
		std::string GetTag() const { return m_Tag; }

		Transform* GetTransform() const { return m_pTransform.get(); }

		Subject<GameObjectEvent> gameObjectDestroyed;

#pragma region Component Logic
		/**
		* @brief Adds a component to the GameObject.
		* @tparam T Type of the component to add.
		* @tparam Args Additional arguments for component construction.
		* @param args Additional arguments forwarded to the component constructor.
		* @return Pointer to the added component.
		*/
		template<typename T, typename... Args>
		// TODO: ask this
		// requires std::is_base_of_v<Component, T>
		T* AddComponent(Args... args);
		/**
		* @brief Gets a component of the GameObject.
		* @tparam T Type of the component to retrieve.
		* @return Pointer to the requested component, or nullptr if not found.
		*/
		template<typename T>
		T* GetComponent();
		template<typename T>
			requires std::is_base_of_v<Component, T>
		T* GetComponentInChildren();
		template<typename T>
			requires std::is_base_of_v<Component, T>
		std::vector<T*> GetComponentsInChildren();
		/**
		 * @brief Removes a component from the GameObject.
		 * @tparam T Type of the component to remove.
		 * @return True if the component was removed successfully, false otherwise.
		 */
		template<typename T>
		bool RemoveComponent();
		/**
		 * @brief Checks if the GameObject has a specific component.
		 * @tparam T Type of the component to check.
		 * @return True if the GameObject has the component, false otherwise.
		 */
		template<typename T>
		bool HasComponent() const;
#pragma endregion
#pragma region Parent Logic
		GameObject* GetParent() const;
		void SetParent(GameObject* pParent, bool keepWorldPosition);
		uint32_t GetChildCount() const;
		std::vector<GameObject*> GetChildren() const;
		GameObject* GetChildAt(uint32_t index) const;
		GameObject* GetChild(uint32_t id) const;
		bool IsChild(GameObject* pGo) const;
#pragma endregion

		std::vector<GameObject*> GetGameObjectsWithTag(const std::string& tag) const;

	private:
		uint32_t m_Id;
		std::string m_Tag;
		bool m_IsActive{ true }, m_IsMarkedForDestroy{ false };
		float m_TimeForDestruction{ -FLT_MAX };

		Scene* m_pScene;

		std::unique_ptr<Transform> m_pTransform{ nullptr };
		std::vector<std::unique_ptr<Component>> m_pComponents{};

		GameObject* m_pParent{ nullptr };
		std::vector<std::unique_ptr<GameObject>> m_pChildren{};
		std::vector<std::unique_ptr<GameObject>> m_pChildrenToAdd{};

#pragma region Component Logic
		template<typename T>
		bool IsTransform() const { return std::is_same_v<T, Transform>; }
#pragma endregion Component Logic

		static inline uint32_t m_IdCounter = 0;
	};

	// TODO: use concepts or smthn, so i dont have to check if it is a concept in code
	template <typename T, typename ... Args>
	T* GameObject::AddComponent(Args... args)
	{
		if (HasComponent<T>())
		{
			Logger::LogError({ "This game object already has a component of this type" });
			throw std::runtime_error("This game object already has a component of this type");
		}

		std::unique_ptr<T> pComponent = std::make_unique<T>(this, std::forward<Args>(args)...);
		T* rawPtr = pComponent.get();
		m_pComponents.emplace_back(std::move(pComponent));

		return rawPtr;
	}

	template <typename T>
	T* GameObject::GetComponent()
	{
		if (IsTransform<T>())
			return dynamic_cast<T*>(m_pTransform.get());

		const auto it = std::find_if(m_pComponents.begin(), m_pComponents.end(), [](const auto& c)
			{
				if (c == nullptr)
					return false;

				return dynamic_cast<T*>(c.get()) != nullptr;
			});

		if (it != m_pComponents.end())
		{
			return dynamic_cast<T*>(it->get());
		}

		//Logger::LogError({"This game object has no component of this type"});
		return nullptr;
	}

	template <typename T>
		requires std::is_base_of_v<Component, T>
	T* GameObject::GetComponentInChildren()
	{
		if (m_pChildren.empty() && m_pChildrenToAdd.empty())
			return nullptr;

		std::ranges::for_each(m_pChildren, [](const std::unique_ptr<GameObject>& go)
			{
				if (const auto component = go->GetComponent<T>();
					component == nullptr)
					return component;
			});

		return nullptr;
	}

	template <typename T>
		requires std::is_base_of_v<Component, T>
	std::vector<T*> GameObject::GetComponentsInChildren()
	{
		std::vector<T*> v;

		if (m_pChildren.empty() == false)
		{
			for (const auto & go : m_pChildren)
			{
				if (go->IsMarkedForDestroy())
					continue;

				auto component = go->GetComponent<T>();
				if (component != nullptr)
				{
					v.push_back(component);
				}

				auto sub = go->GetComponentsInChildren<T>();
				v.insert(v.end(), sub.begin(), sub.end());
			}
		}

		return v;
	}

	template <typename T>
	// TODO: Improve this, use mark for destroy on commands
	bool GameObject::RemoveComponent()
	{
		if (IsTransform<T>())
		{
			Logger::LogWarning({ "The transform component can not be deleted" });
			return false;
		}

		const auto it = std::find_if(m_pComponents.begin(), m_pComponents.end(), [](const auto& c)
			{
				return dynamic_cast<T*>(c.get()) != nullptr;
			});

		if (it != m_pComponents.end())
		{
			std::erase(m_pComponents, it);
			return true;
		}

		Logger::LogError({ "This game object has no component of this type" });
		return false;
	}

	template <typename T>
	bool GameObject::HasComponent() const
	{
		if constexpr (std::is_base_of_v<Component, T> == false)
		{
			Logger::LogError({ "T must derive from Component" });
			return false;
		}

		if (m_pComponents.empty())
			return false;

		if (IsTransform<T>())
			return true;

		const auto it = std::find_if(m_pComponents.begin(), m_pComponents.end(), [](const auto& c)
			{
				// TODO: Find out why this doesnt work
				//return std::is_same_v<T, decltype(c)>;
				return dynamic_cast<T*>(c.get()) != nullptr;
			});

		if (it == m_pComponents.end())
		{
			return false;
		}

		return true;
	}

	//template <typename T>
	//std::vector<std::unique_ptr<Component>>::iterator GameObject::GetComponentIterator()
	//{
	//	// ReSharper disable once CppEntityUsedOnlyInUnevaluatedContext
	//	const auto it = std::find_if(m_pComponents.begin(), m_pComponents.end(), [](const auto& c)
	//		{
	//			return std::is_same_v<T, decltype(*c)>;
	//		});

	//	return it;
	//}
}

#endif // GAMEOBJECT_H