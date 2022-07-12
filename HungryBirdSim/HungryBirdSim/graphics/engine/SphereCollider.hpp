#pragma once

#ifndef BOXCOLLIDER3D_HPP
#define BOXCOLLIDER3D_HPP


#include <glm/glm.hpp>
#include <array>
#include "Collider3D.hpp"

using glm::vec3;
using std::array;

namespace graphics
{
	class SphereCollider : public Collider3D
	{
		public:
		/**
		 * Create a 3D box collider with the specified dimensions.
		 *
		 * @param gameObject the gameObject object to which this collider has to be attached to.
		 * @param size_x the length of the x side of the collider.
		 * @param size_y the length of the y side of the collider.
		 * @param size_z the length of the z side of the collider.
		 */
		SphereCollider(GameObject gameObject, float radius);
		/**
		 * Returns the bounding box of the collider, formed by the 8 vertices of the box.
		 *
		 * @return the coordinates of the 8 vertices of the bounding box.
		 */
		GameObject getGameObject();
		float getSize();
		/**
		 * Check if the given point collides with this box collider.
		 *
		 * @param point the point to check the collision with.
		 * @return true if there is a collision, false otherwise.
		 */
		bool checkCollision(vec3 point);

		private:
		GameObject gameObject;
		float radius;
	};
}

#endif