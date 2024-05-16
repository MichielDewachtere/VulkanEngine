#ifndef NOISEMANAGER_H
#define NOISEMANAGER_H

#include <memory>
#include <cstdint>

#include <real_core/Singleton.h>

#include "Util/SimplexNoise.h"

class NoiseManager final : public real::Singleton<NoiseManager>
{
public:
	virtual ~NoiseManager() override;

	NoiseManager(const NoiseManager& other) = delete;
	NoiseManager& operator=(const NoiseManager& rhs) = delete;
	NoiseManager(NoiseManager&& other) = delete;
	NoiseManager& operator=(NoiseManager&& rhs) = delete;

	void Initialize(uint32_t seed);

	float GetTerrainNoiseValue(double x, double y, double gridSize) const;
	float GetCaveNoiseValue(double x, double y, double z, double gridSize) const;
	float GetSpaghetCaveNoiseValue(double x, double y, double z, double gridSize) const;

private:
	friend class Singleton<NoiseManager>;
	explicit NoiseManager() = default;

	uint32_t m_Seed{ 0 };

	std::unique_ptr<SimplexNoise> m_pTerrainNoise{ nullptr };
	std::unique_ptr<SimplexNoise> m_pCaveNoise{ nullptr };
	std::unique_ptr<SimplexNoise> m_pSpaghetCaveNoise{ nullptr };
};

#endif // NOISEMANAGER_H