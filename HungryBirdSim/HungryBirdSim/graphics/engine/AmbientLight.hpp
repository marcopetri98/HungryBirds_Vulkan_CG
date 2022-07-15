#pragma once

#ifndef AMBIENTLIGHT_HPP
#define AMBIENTLIGHT_HPP

#include <glm/glm.hpp>

#include "Light.hpp"

using glm::vec3;

namespace graphics
{
	class AmbientLight : public Light
	{
		public:
		AmbientLight(vec3 color = vec3(0), vec3 reflection = vec3(0));
		vec3 getLightColor();
		vec3 getLightReflection();

		private:
		vec3 color;
		vec3 reflection;
	};
}

#endif