#pragma once

#ifndef HEMISPHERICALLIGHT_HPP
#define HEMISPHERICALLIGHT_HPP

#include <glm/glm.hpp>

#include "Light.hpp"

using glm::vec3;

namespace graphics
{
	class HemisphericalLight : public Light
	{
		public:
		HemisphericalLight(vec3 topColor = vec3(0), vec3 bottomColor = vec3(0), vec3 topDir = vec3(0, 1, 0), vec3 reflection = vec3(0));
		vec3 getLightColor();
		vec3 getLightBottomColor();
		vec3 getLightTopDir();
		vec3 getLightReflection();

		private:
		vec3 topColor;
		vec3 bottomColor;
		vec3 topDir;
		vec3 reflection;
	};
}

#endif