#include <array>

#include "BoxCollider3D.hpp"

using std::array;

namespace graphics {
	array<float, 3> BoxCollider3D::getBoundingBox()
	{
		// TODO: implement
		return {0.0f, 0.0f, 0.0f};
	}

	GameObject BoxCollider3D::getGameObject()
	{
		// TODO: implement
		return GameObject();
	}
}
