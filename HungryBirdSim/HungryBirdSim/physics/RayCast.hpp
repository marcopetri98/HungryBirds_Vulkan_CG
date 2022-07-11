#pragma once

#ifndef RAYCAST_HPP
#define RAYCAST_HPP

#include "glm/glm.hpp"

using glm::vec3;

namespace physics {
	class RayCast {
		virtual vec3 nextRay(vec3 pos) = 0;
		virtual bool hasNext() = 0;
	};
}


#endif