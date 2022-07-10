#pragma once

#ifndef COLLIDER_HPP
#define COLLIDER_HPP

#include "GameObject.hpp"

using glm::vec3;

namespace graphics
{
	class Collider
	{
		public:
		/**
		 * Gets the game object that owns this collider.
		 * 
		 * @return The game object to which this collider is attached.
		 */
		virtual GameObject getGameObject() = 0;
		virtual bool checkCollision(vec3 point) = 0;
	};
}

#endif