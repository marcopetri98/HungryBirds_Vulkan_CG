#pragma once

#ifndef RAYCAST3D_HPP
#define RAYCAST3D_HPP

#include "glm/glm.hpp"
#include "RayCast.hpp"

using glm::vec3;

namespace physics {
	class RayCast3D : RayCast {
		public:
		RayCast3D(float numRays, float objRadius, float length);
		vec3 nextRay(vec3 pos);
		bool hasNext();

		private:
		float numRays;
		float currAngle;
		float length;
		float objRadius;
		float anglePerStep;
		int currDir;
	};
}


#endif