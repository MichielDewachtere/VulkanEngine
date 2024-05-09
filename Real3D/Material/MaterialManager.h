#ifndef MATERIALMANAGER_H
#define MATERIALMANAGER_H

#include <map>
#include <memory>
#include <algorithm>
#include <ranges>

#include <real_core/Singleton.h>

#include "Util/Structs.h"
#include "Util/Concepts.h"

#include "Material/Material.h"

namespace real
{
	class BaseMaterial;

	class MaterialManager final : public real::Singleton<MaterialManager>
	{
	public:
		virtual ~MaterialManager() override = default;

		MaterialManager(const MaterialManager&) = delete;
		MaterialManager operator=(const MaterialManager&) = delete;
		MaterialManager(MaterialManager&&) = delete;
		MaterialManager operator=(MaterialManager&&) = delete;

		template <pipeline_type P, vertex_type V>
		std::pair<uint8_t, Material<P, V>*> AddMaterial(const GameContext& context);

		template <pipeline_type P, vertex_type V>
		Material<P, V>* GetMaterial() const;
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

template <real::pipeline_type P, real::vertex_type V>
inline std::pair<uint8_t, real::Material<P, V>*> real::MaterialManager::AddMaterial(const GameContext& context)
{
	auto pMat = std::make_unique<Material<P, V>>(context.vulkanContext);
	Material<P, V>* rawPtr = pMat.get();

	m_pMaterials[++m_NextId] = std::move(pMat);

	return { m_NextId, rawPtr };
}

template<real::pipeline_type P, real::vertex_type V>
inline real::Material<P, V>* real::MaterialManager::GetMaterial() const
{
	for (const auto& base : m_pMaterials | std::views::values)
	{
		if (auto mat = dynamic_cast<Material<P, V>*>(base.get()))
			return mat;
	}

	return nullptr;
}
