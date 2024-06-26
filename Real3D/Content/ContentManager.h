#ifndef CONTENTMANAGER_H
#define CONTENTMANAGER_H

#include <map>
#include <memory>

#include <real_core/Singleton.h>

#include "Model.h"
#include "Texture2D.h"
#include "Util/Structs.h"

namespace real
{
	class ContentManager final : public real::Singleton<ContentManager>
	{
	public:
		virtual ~ContentManager() override = default;

		Model* LoadModel(const std::string& path, const glm::vec3& pos);
		Texture2D* LoadTexture(const GameContext& context, const std::string& path);

		void CleanUp(const GameContext& context);

	private:
		friend class Singleton<ContentManager>;
		ContentManager() = default;

		std::map<std::string, std::unique_ptr<Texture2D>> m_Images;
		std::map<std::string, std::unique_ptr<Model>> m_Models;

		//template<typename T, typename... Args>
		//T* LoadContent(std::map<std::string, std::unique_ptr<T>>&, const std::string& path, Args... args);
	};
}

#endif // CONTENTMANAGER_H