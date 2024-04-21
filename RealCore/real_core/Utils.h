#ifndef REALUTILS_H
#define REALUTILS_H

#include <string>
#include <glm/vec2.hpp>

//static SDL_Color I8Vec4ToColor(const glm::i8vec4& vec)
//{
//	return SDL_Color{
//		static_cast<Uint8>(vec.r),
//		static_cast<Uint8>(vec.g),
//		static_cast<Uint8>(vec.b),
//		static_cast<Uint8>(vec.a)
//	};
//}

static inline std::string Vec2ToString(const glm::vec2& vec)
{
	std::string s = "{ " + std::to_string(vec.x) + " ; " + std::to_string(vec.y) + " }";
	return s;
}

#endif // REALUTILS_H