#pragma once

#ifndef SPHERICALLIGHT_HPP
#define SPHERICALLIGHT_HPP

#include <glm/glm.hpp>

#include "Light.hpp"

using glm::vec3;

namespace graphics
{
	class SphericalLight : public Light
	{
		public:
		SphericalLight(vec3 colorDeviationX = vec3(0), vec3 colorDeviationY = vec3(0), vec3 colorDeviationZ = vec3(0), vec3 reflection = vec3(0));
		vec3 getLightColor();
		vec3 getLightDeviationYColor();
		vec3 getLightDeviationZColor();
		vec3 getLightReflection();

		private:
		vec3 colorDeviationX;
		vec3 colorDeviationY;
		vec3 colorDeviationZ;
		vec3 reflection;
	};
}

#endif