#pragma once

#ifndef POINTLIGHT_HPP
#define POINTLIGHT_HPP

#include <glm/glm.hpp>

#include "Light.hpp"

using glm::vec3;

namespace graphics
{
	class PointLight : public Light
	{
		public:
		PointLight(vec3 position, vec3 color, float distanceReduction = 1.0f, float decay = 1.0f);
		vec3 getLightPos();
		vec3 getLightColor();
		float getDecay();
		float getDistanceReduction();

		private:
		vec3 position;
		vec3 color;
		float decay;
		float distanceReduction;
	};
}

#endif
