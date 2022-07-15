#pragma once

#ifndef PLANECOLLIDER3D_HPP
#define PLANECOLLIDER3D_HPP


#include <glm/glm.hpp>
#include <array>
#include "Collider3D.hpp"
#include "GameObject.hpp"

using glm::vec3;
using std::array;

namespace graphics
{
	class PlaneCollider3D : public Collider3D
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
		PlaneCollider3D(GameObject* gameobject, float size_x, float size_z);

		/**
		 * Returns the bounding box of the collider, formed by the 8 vertices of the box.
		 *
		 * @return the coordinates of the 8 vertices of the bounding box.
		 */
		array<vec3, 4> getBoundingBox();
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
		float size_x, size_z;
		float x_low, x_high, z_low, z_high;
		array<vec3, 4> boundingBox;

		/**
		 * Create the bounding box for the collider, composed by the 8 vertices of the box.
		 *
		 */
		void createBoundingBox();

	};
}

#endif