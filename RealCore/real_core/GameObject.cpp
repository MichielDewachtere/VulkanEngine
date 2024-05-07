#include "GameObject.h"

#include "DrawableComponent.h"
#include "GameTime.h"
#include "imgui.h"
#include "InputManager.h"
#include "Logger.h"
#include "SceneManager.h"

real::GameObject::GameObject(Scene* scene, TransformInfo info, std::string tag)
	: m_Tag(std::move(tag))
	, m_pScene(scene)
{
	m_Id = m_IdCounter++;
	m_pTransform = std::make_unique<Transform>(this, info);
}

real::GameObject& real::GameObject::CreateGameObject(TransformInfo info, std::string tag)
{
	m_pChildrenToAdd.push_back(std::make_unique<GameObject>(m_pScene, info, std::move(tag)));
	m_pChildrenToAdd.back()->m_pParent = this;
	m_pChildrenToAdd.back()->m_pScene = m_pScene;

	return *m_pChildrenToAdd.back();
}

void real::GameObject::Start()
{
	m_pTransform->Start();
	std::ranges::for_each(m_pComponents, [](const std::unique_ptr<Component>& c)
		{
			c->Start();
		});

	if (m_pChildrenToAdd.empty() == false)
	{
		for (auto& pChild : m_pChildrenToAdd)
		{
			m_pChildren.push_back(std::move(pChild));
		}

		m_pChildrenToAdd.clear();
	}

	if (m_pChildren.empty() == false)
	{
		std::ranges::for_each(m_pChildren, [](const auto& go)
			{
				go->Start();
			});
	}
}

void real::GameObject::FixedUpdate()
{
	if (IsActive() == false)
		return;

	m_pTransform->FixedUpdate();
	std::ranges::for_each(m_pComponents, [](const std::unique_ptr<Component>& c)
		{
			if (c->IsActive())
				c->FixedUpdate();
		});

	if (m_pChildren.empty() == false)
	{
		std::ranges::for_each(m_pChildren, [](const auto& go)
			{
				go->FixedUpdate();
			});
	}
}

void real::GameObject::Update()
{
	if (IsActive() == false)
		return;

	m_pTransform->Update();
	std::ranges::for_each(m_pComponents, [](const std::unique_ptr<Component>& c)
		{
			if (c->IsActive())
				c->Update();
		});

	if (m_pChildren.empty() == false)
	{
		std::ranges::for_each(m_pChildren, [](const auto& go)
			{
				go->Update();
			});
	}
}

void real::GameObject::LateUpdate()
{
	MoveUniqueData(m_pChildrenToAdd, m_pChildren);
	MoveUniqueData(m_pComponentsToAdd, m_pComponents);

	if (m_TimeForDestruction > 0)
	{
		m_TimeForDestruction -= GameTime::GetInstance().GetElapsed();
		if (m_TimeForDestruction <= 0)
		{
			Destroy();
		}
	}

	if (m_IsMarkedForDestroy && m_TimeForDestruction <= 0)
	{
		for (const auto& c : m_pComponents)
		{
			c->Kill();
		}

		m_pComponents.clear();

		if (m_pParent == nullptr)
			m_pScene->Remove(this);
	}

	if (m_pChildren.empty() == false)
	{
		for (auto it = m_pChildren.begin(); it != m_pChildren.end(); )
		{
			const auto& pChild = *it;

			//if (IsActive())
			pChild->LateUpdate();

			if (pChild->IsMarkedForDestroy() && pChild->GetId() <= m_IdCounter)
			{
				it->reset();
				it = m_pChildren.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

	if (IsActive())
	{
		m_pTransform->LateUpdate();
		std::ranges::for_each(m_pComponents, [](const std::unique_ptr<Component>& c)
			{
				if (c->IsActive())
					c->LateUpdate();
			});
	}
}

void real::GameObject::Render() const
{
	if (IsActive() == false)
		return;

	if (m_pChildren.empty() == false)
	{
		std::ranges::for_each(m_pChildren, [](const auto& go)
			{
				go->Render();
			});
	}

	std::ranges::for_each(m_pComponents, [](const std::unique_ptr<Component>& c)
		{
			if (const auto drawable = dynamic_cast<DrawableComponent*>(c.get());
				drawable != nullptr)
			{
				if (drawable->IsActive())
					drawable->Render();
			}
		});
}

void real::GameObject::DebugRender() const
{
	if (IsActive() == false)
		return;

	if (m_pChildren.empty() == false)
	{
		std::ranges::for_each(m_pChildren, [](const auto& go)
			{
				go->DebugRender();
			});
	}

	std::ranges::for_each(m_pComponents, [](const std::unique_ptr<Component>& c)
		{
			if (const auto drawable = dynamic_cast<DrawableComponent*>(c.get());
				drawable != nullptr)
			{
				if (drawable->IsActive())
					drawable->DebugRender();
			}
		});
}

void real::GameObject::OnGui()
{
	if (IsActive() == false)
		return;

	ImGui::PushID(m_Id);
	ImGui::Text(std::to_string(m_Id).c_str());

	m_pTransform->OnGui();
	std::ranges::for_each(m_pComponents, [](const std::unique_ptr<Component>& c)
		{
			c->OnGui();
		});

	if (m_pChildren.empty() == false)
	{
		std::ranges::for_each(m_pChildren, [](const auto& go)
			{
				go->OnGui();
			});
	}

	ImGui::PopID();
}

void real::GameObject::Destroy()
{
	gameObjectDestroyed.Notify(GameObjectEvent::destroyed);
	InputManager::GetInstance().RemoveGameObjectCommands(this);

	std::ranges::for_each(m_pChildren, [](const auto& go)
		{
			go->Destroy();
		});

	m_IsMarkedForDestroy = true;
}

void real::GameObject::Destroy(float time)
{
	m_TimeForDestruction = std::max(0.f, time);
}

void real::GameObject::SetIsActive(bool isEnabled, bool applyToChildren)
{
	if (m_IsActive != isEnabled)
	{
		m_IsActive = isEnabled;

		if (applyToChildren)
		{
			std::ranges::for_each(m_pChildren, [&](const auto& c)
			   {
				   c->SetIsActive(isEnabled, applyToChildren);
			   });
		}
	}
}

void real::GameObject::SetScene(Scene* pScene)
{
	m_pScene = pScene;

	std::ranges::for_each(m_pChildren, [&](const auto& c)
		{
			c->SetScene(pScene);
		});
	if (m_pChildrenToAdd.empty() == false)
	{
		std::ranges::for_each(m_pChildrenToAdd, [&](const auto& c)
		   {
			   c->SetScene(pScene);
		   });
	}
}

real::GameObject* real::GameObject::GetParent() const
{
	return m_pParent;
}

void real::GameObject::SetParent(GameObject* pParent, bool keepWorldPosition)
{
	if (IsChild(pParent) || pParent == this || m_pParent == pParent)
	{
		if (pParent == nullptr)
			Logger::LogWarning({ "Couldn't attach game object ( id: {} ) as a child, passed parent game object was NULL" }, m_Id);
		else
			Logger::LogWarning({ "Couldn't attach game object ( id: {} ) as a child to game object ( id: {} )" }, m_Id, pParent->GetId());

		return;
	}

	if (m_pParent != nullptr)
	{
		for (int i = 0; i < static_cast<int>(m_pParent->m_pChildren.size()); ++i)
		{
			if (m_pParent->m_pChildren[i]->m_Id == m_Id)
			{
				// If pParent is nullptr, the scene should own the game object
				if (pParent == nullptr)
					m_pScene->AddGameObject(std::move(m_pParent->m_pChildren[i]));
				// else, the defined parent should own the game object
				else
					pParent->m_pChildren.push_back(std::move(m_pParent->m_pChildren[i]));

				// Remove the dangling pointer
				m_pParent->m_pChildren[i] = std::move(m_pParent->m_pChildren[m_pParent->m_pChildren.size() - 1]);
				m_pParent->m_pChildren.pop_back();

				// Set the current parent
				m_pParent = pParent;
				break;
			}
		}
	}
	else
	{
		m_pParent = pParent;
		// Move the game object from the scene to the new parent
		m_pParent->m_pChildren.push_back(std::move(m_pScene->GetUniqueGameObject(this)));
	}

	// Update the position
	if (keepWorldPosition && m_pParent != nullptr)
		m_pTransform->SetWorldPosition(m_pTransform->GetLocalPosition() + m_pParent->GetTransform()->GetWorldPosition());
	else
		m_pTransform->SetLocalPosition(m_pTransform->GetWorldPosition());
}

uint32_t real::GameObject::GetChildCount() const
{
	return static_cast<uint32_t>(m_pChildren.size());
}

std::vector<real::GameObject*> real::GameObject::GetChildren() const
{
	std::vector<GameObject*> v{};

	std::ranges::for_each(m_pChildren, [&v](const auto& c)
	{
		v.push_back(c.get());
	});

	return v;
}

real::GameObject* real::GameObject::GetChildAt(uint32_t index) const
{
	if (index < m_pChildren.size())
		return m_pChildren[index].get();

	if (index < m_pChildrenToAdd.size() + m_pChildren.size())
		return m_pChildrenToAdd[index + m_pChildren.size()].get();

	return nullptr;
}

real::GameObject* real::GameObject::GetChild(uint32_t id) const
{
	for (const auto& child : m_pChildrenToAdd)
	{
		if (child->GetId() == id)
			return child.get();

	}

	for (const auto& child : m_pChildren)
	{
		if (child->GetId() == id)
			return child.get();


		if (const auto pGo = child->GetChild(id);
			pGo != nullptr)
			return pGo;
	}

	return nullptr;
}

bool real::GameObject::IsChild(GameObject* pGo) const
{
	return std::ranges::any_of(m_pChildren, [pGo](const auto& pChild)
		{
			return pChild->GetId() == pGo->GetId();
		});
}

std::vector<real::GameObject*> real::GameObject::GetGameObjectsWithTag(const std::string& tag) const
{
	std::vector<GameObject*> v;

	auto c = GetGameObjectsWithTagHelper(m_pChildren, tag);
	v.insert(v.end(), c.begin(), c.end());
	c = GetGameObjectsWithTagHelper(m_pChildrenToAdd, tag);
	v.insert(v.end(), c.begin(), c.end());

	return v;
}

std::vector<real::GameObject*> real::GameObject::GetGameObjectsWithTagHelper(
	const std::vector<std::unique_ptr<GameObject>>& objects, const std::string& tag)
{
	std::vector<GameObject*> v;

	if (objects.empty() == false)
	{
		for (const auto& go : objects)
		{
			//if (go->IsMarkedForDestroy())
			//	continue;

			if (go->GetTag() == tag)
				v.push_back(go.get());

			auto sub = go->GetGameObjectsWithTag(tag);
			v.insert(v.end(), sub.begin(), sub.end());
		}
	}

	return v;
}
