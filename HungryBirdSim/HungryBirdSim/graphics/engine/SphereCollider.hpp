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
		 * Create a 3D sphere collider with the specified dimensions.
		 *
		 * @param radius the radius of the sphere.
		 */
		SphereCollider(float radius = 0.0f);
		/**
		 * Returns the bounding box of the collider, formed by the 8 vertices of the box.
		 *
		 * @return the coordinates of the 8 vertices of the bounding box.
		 */
		GameObject getGameObject();
		void setGameObject(GameObject* gameObject);
		float getSize();
		/**
		 * Check if the given point collides with this box collider.
		 *
		 * @param point the point to check the collision with.
		 * @return true if there is a collision, false otherwise.
		 */
		bool checkCollision(vec3 point);

		private:
		GameObject* gameObject;
		float radius;
	};
}

#endif