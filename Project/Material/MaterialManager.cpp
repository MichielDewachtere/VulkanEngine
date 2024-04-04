#include "MaterialManager.h"

#include "BaseMaterial.h"

BaseMaterial* MaterialManager::GetMaterial(uint8_t id) const
{
    return m_pMaterials.at(id).get();
}

std::vector<BaseMaterial*> MaterialManager::GetMaterials() const
{
    std::vector<BaseMaterial*> v;

    std::ranges::transform(m_pMaterials, std::back_inserter(v),
        [](const auto& pair) { return pair.second.get(); });

    return v;
}

void MaterialManager::RemoveMaterial(const GameContext& context, uint8_t id)
{
    // TODO: Clean up
    m_pMaterials.at(id)->CleanUp(context);
    m_pMaterials.erase(id);
}

void MaterialManager::RemoveMaterials(const GameContext& context)
{
    for (const auto& mat : m_pMaterials | std::views::values)
        mat->CleanUp(context);

	m_pMaterials.clear();
}
