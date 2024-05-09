#include "ContentManager.h"

#include <ranges>
#include <stdexcept>

real::Texture2D* real::ContentManager::LoadTexture(const GameContext& context, const std::string& path)
{
	if (m_Images.contains(path))
	{
		return m_Images.at(path).get();
	}

	m_Images.emplace(path, std::make_unique<Texture2D>(path, context));
	return m_Images.at(path).get();
}

real::Model* real::ContentManager::LoadModel(const std::string& path, const glm::vec3& pos)
{
	if (m_Models.contains(path))
	{
		return m_Models.at(path).get();
	}

	m_Models.emplace(path, std::make_unique<Model>(path, pos));
	return m_Models.at(path).get();
}

void real::ContentManager::CleanUp(const GameContext& context)
{
	for (const auto& texture : m_Images | std::views::values)
	{
		texture->CleanUp(context);
	}

	m_Images.clear();
	m_Models.clear();
}
