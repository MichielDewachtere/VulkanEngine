#include "NoiseManager.h"

#include "SimplexNoise.h"

NoiseManager::~NoiseManager() = default;

void NoiseManager::Initialize(uint32_t seed)
{
	m_Seed = seed;
	m_pTerrainNoise = std::make_unique<SimplexNoise>(0.25f, 12.f);
	m_pCaveNoise = std::make_unique<SimplexNoise>(0.1f);
	m_pSpaghetCaveNoise = std::make_unique<SimplexNoise>(0.4f);
}

float NoiseManager::GetTerrainNoiseValue(double x, double y, double gridSize) const
{
	return m_pTerrainNoise->fractal(4, x / gridSize, y / gridSize);
}

float NoiseManager::GetCaveNoiseValue(double x, double y, double z, double gridSize) const
{
	return m_pCaveNoise->fractal(1, x / gridSize, y / gridSize, z / gridSize);
}

float NoiseManager::GetSpaghetCaveNoiseValue(double x, double y, double z, double gridSize) const
{
	return m_pSpaghetCaveNoise->fractal(1, x / gridSize, y / gridSize, z / gridSize);
}
