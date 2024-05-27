#ifndef FLUIDPARSER_H
#define FLUIDPARSER_H

#include <vector>

#include <real_core/Singleton.h>
#include <Util/Structs.h>

#include "Enumerations.h"


class FluidParser final : public real::Singleton<FluidParser>
{
public:
	virtual ~FluidParser() override = default;

	FluidParser(const FluidParser& other) = delete;
	FluidParser& operator=(const FluidParser& rhs) = delete;
	FluidParser(FluidParser&& other) = delete;
	FluidParser& operator=(FluidParser&& rhs) = delete;

	std::vector<real::PosTexNorm> GetFaceData(EDirection dir, glm::vec3 pos, bool waterAbove) const;

private:
	friend class Singleton<FluidParser>;
	explicit FluidParser() = default;

	float m_TextureSize{ 16.f }, m_SheetHeight{ 512.f };

	static std::vector<glm::vec3> GetVertexPositions(EDirection dir, bool waterAbove);
	glm::vec2 GetTexCoord(int i) const;
};

#endif // FLUIDPARSER_H