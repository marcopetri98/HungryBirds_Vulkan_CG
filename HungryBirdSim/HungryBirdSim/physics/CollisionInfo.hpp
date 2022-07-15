#pragma once

#ifndef COLLISIONINFO_HPP
#define COLLISIONINFO_HPP

#include <glm/glm.hpp>
#include "../graphics/engine/DummyRecursionSolver.hpp"
#include "../graphics/engine/GameObject.hpp"

using glm::vec3;
using graphics::GameObject;

namespace physics {
	/**
	 * A struct containing info about a collision, if happened.
	 */
	struct CollisionInfo{
		public:
		CollisionInfo(bool collided, GameObject* collidedObject, vec3 collisionPoint, vec3 collisionDir) {
			this->collided = collided;
			this->collidedObject = collidedObject;
			this->collisionPoint = collisionPoint;
			this->collisionDir = collisionDir;
		}
		CollisionInfo() {
			this->collided = false;
		}
		bool collided;
		GameObject* collidedObject;
		vec3 collisionPoint;
		vec3 collisionDir;
	};
}


#endif
