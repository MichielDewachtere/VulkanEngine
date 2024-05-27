#include "TransparentModel.h"

#include <algorithm>

#include "Core/CommandBuffers/CommandBuffer.h"

#include "Material/MaterialManager.h"
#include "real_core/GameTime.h"
#include "Util/GameInfo.h"

TransparentModel::TransparentModel(real::GameObject* pOwner, uint32_t vertexCapacity, uint32_t indexCapacity)
	: DrawableComponent(pOwner), m_VertexCapacity(vertexCapacity), m_IndexCapacity(indexCapacity)
{
	m_VertexBuffers.push_back({});
	m_IndexBuffers.push_back({});

	if (m_VertexCapacity == 0) m_VertexCapacity = 512;
	if (m_IndexCapacity == 0) m_IndexCapacity = 512;

	CreateBuffer<real::PosTexNorm>(m_VertexBuffers, 0, m_VertexCapacity, true);
	CreateBuffer<uint32_t>(m_IndexBuffers, 0, m_IndexCapacity, false);

	m_pTransparentMaterial = real::MaterialManager::GetInstance().GetMaterial<TransparentMaterial>();
	m_pWaterMaterial = real::MaterialManager::GetInstance().GetMaterial<WaterMaterial>();
	m_pTranspriteMaterial = real::MaterialManager::GetInstance().GetMaterial<TranspriteMaterial>();

	m_TransparentReference = m_pTransparentMaterial->AddReference();
	m_WaterReference = m_pWaterMaterial->AddReference();
	m_TranspriteReference = m_pTranspriteMaterial->AddReference();
}

TransparentModel::~TransparentModel()
{
	const auto context = real::RealEngine::GetGameContext();

	for (auto& [isDirty, buffer, allocation, data] : m_IndexBuffers)
	{
		if (buffer == nullptr)
			continue;

		vmaDestroyBuffer(context.vulkanContext.allocator, buffer, allocation);
		buffer = nullptr;
	}

	for (auto& [isDirty, buffer, allocation, data] : m_VertexBuffers)
	{
		if (buffer == nullptr)
			continue;

		vmaDestroyBuffer(context.vulkanContext.allocator, buffer, allocation);
		buffer = nullptr;
	}
}

void TransparentModel::Update()
{
	if (m_BuffersAreDirty == false)
		return;

	for (auto& [isDirty, buffer, memory, data] : m_VertexBuffers)
	{
		//if (isDirty)
		//{
			UpdateBuffer<real::PosTexNorm>(m_Vertices, buffer, m_VertexCapacity);
			isDirty = false;
		//}
	}

	for (auto& [isDirty, buffer, memory, data] : m_IndexBuffers)
	{
		//if (isDirty)
		//{
			UpdateBuffer<uint32_t>(m_Indices, buffer, m_IndexCapacity);
			isDirty = false;
		//}
	}

	m_BuffersAreDirty = false;
}

void TransparentModel::Render()
{
	const auto commandBuffer = real::CommandPool::GetInstance().GetActiveCommandBuffer();

	m_pTransparentMaterial->UpdateShaderVariables(this, m_TransparentReference);
	m_pWaterMaterial->UpdateShaderVariables(this, m_WaterReference);
	m_pTranspriteMaterial->UpdateShaderVariables(this, m_TranspriteReference);

	for (const auto& [begin, end, type] : m_Regions)
	{
		switch (type)
		{
		case TransparencyType::water:
		{
			m_pWaterMaterial->Bind(commandBuffer, m_WaterReference);
			break;
		}
		case TransparencyType::transparentTexture:
		{
			m_pTransparentMaterial->Bind(commandBuffer, m_TransparentReference);
			break;
		}
		case TransparencyType::transparentSprite:
		{
			m_pTranspriteMaterial->Bind(commandBuffer, m_TranspriteReference);
			break;
		}
		}

		const VkBuffer vertexBuffers[] = { m_VertexBuffers.front().buffer };
		const VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
		
		vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffers.front().buffer, 0, VK_INDEX_TYPE_UINT32);
		vkCmdDrawIndexed(commandBuffer, end - begin, 1, begin, 0, 0);
	}
}

void TransparentModel::Kill()
{
	const auto context = real::RealEngine::GetGameContext();

	for (auto& [isDirty, buffer, allocation, data] : m_IndexBuffers)
	{
		vmaDestroyBuffer(context.vulkanContext.allocator, buffer, allocation);
		buffer = nullptr;
	}

	for (auto& [isDirty, buffer, allocation, data] : m_VertexBuffers)
	{
		vmaDestroyBuffer(context.vulkanContext.allocator, buffer, allocation);
		buffer = nullptr;
	}

	m_pTransparentMaterial->CleanUpUbo(m_TransparentReference);
	m_pWaterMaterial->CleanUpUbo(m_WaterReference);
	m_pTranspriteMaterial->CleanUpUbo(m_TranspriteReference);
}

void TransparentModel::AddFaces(const std::vector<TransparentFace>& faces)
{
	m_Faces.insert(m_Faces.end(), faces.begin(), faces.end());
	m_BuffersAreDirty = true;
}

void TransparentModel::SetFaces(const std::vector<TransparentFace>& faces)
{
	ClearFaces();
	AddFaces(faces);
}

void TransparentModel::ClearFaces()
{
	m_Faces.clear();
	m_BuffersAreDirty = true;
}

void TransparentModel::SortFaces(const glm::ivec3& position, bool sortBlocks)
{
	m_Vertices.clear();
	m_Indices.clear();
	m_Regions.clear();

	if (m_Faces.empty())
		return;

	if (sortBlocks)
	{
		std::ranges::sort(m_Faces, [&position](const TransparentFace& a, const TransparentFace& b)
			{
				auto distanceSquared = [](const glm::ivec2& p1, const glm::ivec2& p2)
					{
						const glm::ivec2 diff = p1 - p2;
						return diff.x * diff.x + diff.y * diff.y;
					};

				return distanceSquared(a.center, position) > distanceSquared(b.center, position);
			});
	}
	else
	{
		std::ranges::sort(m_Faces, [](const TransparentFace& a, const TransparentFace& b)
			{
				return a.type < b.type;
			});
	}

	auto currentType = TransparencyType::none;
	auto currentIdx = 0, prevIdx = 0;
	auto offset = 0;
	for (const auto& face : m_Faces)
	{
		if (currentType != face.type)
		{
			m_Regions.emplace_back(prevIdx, currentIdx, currentType);

			currentType = face.type;
			prevIdx = currentIdx;
		}

		m_Vertices.insert(m_Vertices.begin(), face.vertices.begin(), face.vertices.end());
		m_Indices.insert(m_Indices.begin(), {
							 (uint32_t)0 + offset, (uint32_t)1 + offset, (uint32_t)2 + offset, (uint32_t)2 + offset,
							 (uint32_t)3 + offset, (uint32_t)0 + offset });

		currentIdx += 6;
		offset += 4;
	}

	m_Regions.emplace_back(prevIdx, currentIdx, currentType);
	m_BuffersAreDirty = true;
}

void TransparentModel::CopyBuffer(const real::GameContext& context, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	const auto commandBuffer = real::CommandBuffer::StartSingleTimeCommands(context);

	VkBufferCopy copyRegion{};
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	real::CommandBuffer::StopSingleTimeCommands(context, commandBuffer);
}