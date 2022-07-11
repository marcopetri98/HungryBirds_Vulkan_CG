#pragma once

#ifndef RAYCAST2D_HPP
#define RAYCAST2D_HPP

#include "glm/glm.hpp"
#include "RayCast.hpp"

using glm::vec3;

namespace physics {
	class RayCast2D: public RayCast {
		public:
		RayCast2D();
		RayCast2D(float numRays, vec3 axis, float objRadius, float length);
		/**
		 * Get the direction and position of the next ray.
		 *
		 * @param pos The position of the center of the object to start raycasting from.
		 * @return A pair containing the direction and the position of the next ray.
		*/
		pair<vec3, vec3> nextRay(vec3 pos);
		/**
		 * Check if the raycasting has explored all available raycasts.
		 *
		 * @return true if there is at least another ray to cast.
		 */
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