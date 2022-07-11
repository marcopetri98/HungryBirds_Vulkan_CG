#pragma once

#ifndef RAYCAST2D_HPP
#define RAYCAST2D_HPP

#include "glm/glm.hpp"
#include "RayCast.hpp"

using glm::vec3;

namespace physics {
	class RayCast2D: RayCast {
		public:
		RayCast2D(float numRays, vec3 axis, float objRadius, float length);
		vec3 nextRay(vec3 pos);
		bool hasNext();

		private:
		float numRays;
		float currAngle;
		float length;
		float objRadius;
		float anglePerStep;
		vec3 axis;


	};
}


#endif