#ifndef COLORS_H
#define COLORS_H

#include <glm/vec4.hpp>

namespace real
{
	struct Colors
	{
		inline static const glm::u8vec4 red = { 255,0,0,255 };
		inline static const glm::u8vec4 green = { 0,255,0,255 };
		inline static const glm::u8vec4 blue = { 0,0,255,255 };
		inline static const glm::u8vec4 white = { 255,255,255,255 };
		inline static const glm::u8vec4 black = { 0,0,0,255 };
		inline static const glm::u8vec4 yellow = { 255,255,0,255 };
		inline static const glm::u8vec4 cyan = { 0,255,255,255 };
		inline static const glm::u8vec4 purple = { 255,0,255,255 };

		inline static const glm::u8vec4 light_grey = { 211,211,211,255 };
		inline static const glm::u8vec4 grey = { 128,128,128,255 };
		inline static const glm::u8vec4 dark_grey = { 105,105,105,255 };


		inline static const glm::u8vec4 appelblauwzeegroen = { 0,237,116,255 };
		inline static const glm::u8vec4 pink = { 204,164,203,255 };
	};
}

#endif // COLORS_H
