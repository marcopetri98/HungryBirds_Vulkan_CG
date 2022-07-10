#pragma once

#ifndef COLLIDER3D_HPP
#define COLLIDER3D_HPP

#include <array>

#include "Collider.hpp"

using std::array;
using glm::vec3;

namespace graphics
{
	class Collider3D : Collider
	{
		public:
		/**
		 * Gets the bounding box of the collider.
		 * 
		 * @return An array containing: width, height and depth respectively
		 */
		virtual bool checkCollision(vec3 point) = 0;
	};
}

#endif