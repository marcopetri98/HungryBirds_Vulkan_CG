#pragma once

#ifndef BOXCOLLIDER3D_HPP
#define BOXCOLLIDER3D_HPP

#include <array>

#include "Collider3D.hpp"

using std::array;

namespace graphics
{
	class BoxCollider3D : Collider3D
	{
		public:
		array<float, 3> getBoundingBox();
		GameObject getGameObject();
	};
}

#endif