#pragma once

#ifndef COLLIDER_HPP
#define COLLIDER_HPP

#include "GameObject.hpp"

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
	};
}

#endif