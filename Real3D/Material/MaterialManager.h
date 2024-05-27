#ifndef MATERIALMANAGER_H
#define MATERIALMANAGER_H

#include <map>
#include <memory>
#include <algorithm>
#include <ranges>

#include <real_core/Singleton.h>

#include "Util/Structs.h"
#include "Util/Concepts.h"

#include "BaseMaterial.h"

namespace real
{
	class MaterialManager final : public real::Singleton<MaterialManager>
	{
	public:
		virtual ~MaterialManager() override = default;

		MaterialManager(const MaterialManager&) = delete;
		MaterialManager operator=(const MaterialManager&) = delete;
		MaterialManager(MaterialManager&&) = delete;
		MaterialManager operator=(MaterialManager&&) = delete;

		void CleanUp();

		template <typename T>
		std::pair<uint8_t, T*> AddMaterial(const GameContext& context);

		template <typename T>
		T* GetMaterial() const;
		BaseMaterial* GetMaterial(uint8_t id) const;
		std::vector<BaseMaterial*> GetMaterials() const;

		void RemoveMaterial(const GameContext& context, uint8_t id);
		void RemoveMaterials(const GameContext& context);

	private:
		friend class Singleton<MaterialManager>;
		MaterialManager() = default;

		std::map<uint8_t, std::unique_ptr<BaseMaterial>> m_pMaterials;

		static inline uint8_t m_NextId{ 0 };
	};
}

#endif // MATERIALMANAGER_H

template <typename T>
std::pair<uint8_t, T*> real::MaterialManager::AddMaterial(const GameContext& context)
{
	auto pMat = std::make_unique<T>();
	T* rawPtr = pMat.get();
	pMat->Init();

	m_pMaterials[++m_NextId] = std::move(pMat);

	return { m_NextId, rawPtr };
}

template <typename T>
T* real::MaterialManager::GetMaterial() const
{
	for (const auto& base : m_pMaterials | std::views::values)
	{
		if (auto mat = dynamic_cast<T*>(base.get()))
			return mat;
	}

	return nullptr;
}