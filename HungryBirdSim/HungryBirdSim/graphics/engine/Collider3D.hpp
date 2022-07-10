#pragma once

#ifndef COLLIDER3D_HPP
#define COLLIDER3D_HPP

#include <array>

#include "Collider.hpp"

using std::array;

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
		virtual array<float, 3> getBoundingBox() = 0;
	};
}

#endif