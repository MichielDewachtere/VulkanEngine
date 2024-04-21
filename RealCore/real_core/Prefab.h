#ifndef PREFAB_H
#define PREFAB_H

namespace real
{
	class Scene;
	class GameObject;

	class Prefab
	{
	public:
		explicit Prefab(GameObject* pOwner);
		explicit Prefab(Scene* pScene);
		virtual ~Prefab() = default;

		Prefab(const Prefab& other) = delete;
		Prefab& operator=(const Prefab& rhs) = delete;
		Prefab(Prefab&& other) = delete;
		Prefab& operator=(Prefab&& rhs) = delete;

		GameObject* GetGameObject() const { return m_pGameObject; }

	private:
		GameObject* m_pGameObject{ nullptr };
	};
}

#endif // PREFAB_H