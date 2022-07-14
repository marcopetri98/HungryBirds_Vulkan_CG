#pragma once

#ifndef DIRECTIONALLIGHT_HPP
#define DIRECTIONALLIGHT_HPP

#include <glm/glm.hpp>

#include "Light.hpp"

using glm::vec3;

namespace graphics
{
	class DirectionalLight : public Light
	{
		public:
		DirectionalLight(vec3 direction, vec3 color);
		vec3 getLightDir();
		vec3 getLightColor();

		private:
		vec3 direction;
		vec3 color;
	};
}

#endif
