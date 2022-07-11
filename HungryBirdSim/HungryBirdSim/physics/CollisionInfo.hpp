#pragma once

#ifndef COLLISIONINFO_HPP
#define COLLISIONINFO_HPP

#include <glm/glm.hpp>
#include "../graphics/engine/DummyRecursionSolver.hpp"
#include "../graphics/engine/GameObject.hpp"

using glm::vec3;
using graphics::GameObject;

namespace physics {
	struct CollisionInfo{
		bool collided = false;
		GameObject collidedObject;
		vec3 collisionPoint;
		vec3 collisionDir;
	};
}


#endif
