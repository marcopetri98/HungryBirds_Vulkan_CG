#pragma once

#ifndef RAYCAST_HPP
#define RAYCAST_HPP

#include "glm/glm.hpp"

using glm::vec3;
using std::pair;

namespace physics {
	class RayCast {
		public:
		virtual pair<vec3, vec3> nextRay(vec3 pos) = 0;
		virtual bool hasNext() = 0;
	};
}


#endif