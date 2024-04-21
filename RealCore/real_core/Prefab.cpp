#include "Prefab.h"

#include "GameObject.h"
#include "Scene.h"

real::Prefab::Prefab(GameObject* pOwner)
{
	m_pGameObject = &pOwner->CreateGameObject();
}

real::Prefab::Prefab(Scene* pScene)
{
	m_pGameObject = &pScene->CreateGameObject();
}
