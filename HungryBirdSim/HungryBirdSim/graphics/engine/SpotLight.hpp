#pragma once

#ifndef SPOTLIGHT_HPP
#define SPOTLIGHT_HPP

#include <glm/glm.hpp>

#include "Light.hpp"

using glm::vec3;

namespace graphics
{
	class SpotLight : public Light
	{
		public:
		SpotLight(vec3 direction, vec3 position, vec3 color, float distanceReduction = 1.0f, float decay = 1.0f, float cosineOuter = 30.0f, float cosineInner = 10.0f);
		vec3 getLightDir();
		vec3 getLightPos();
		vec3 getLightColor();
		float getDecay();
		float getDistanceReduction();
		float getCosineOuterAngle();
		float getCosineInnerAngle();

		private:
		vec3 direction;
		vec3 position;
		vec3 color;
		float decay;
		float distanceReduction;
		float cosineOuter;
		float cosineInner;
	};
}

#endif
