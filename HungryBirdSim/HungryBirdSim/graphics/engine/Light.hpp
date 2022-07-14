#pragma once

#ifndef LIGHT_HPP
#define LIGHT_HPP

#include <glm/glm.hpp>

using glm::vec3;

namespace graphics
{
	class Light
	{
		public:
		virtual vec3 getLightColor() = 0;
	};
}

#endif
