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
	class BoxCollider3D : Collider3D
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
		BoxCollider3D(GameObject gameObject, float size_x, float size_y = 0, float size_z = 0);
		/**
		 * Create a 3D box collider with a square base of with the given side and a given height.
		 * 
		 * @param gameObject the gameObject object to which this collider has to be attached to.
		 * @param side the side of the square base.
		 * @param height the height of the collider.
		 */
		BoxCollider3D(GameObject gameObject, float side, float height);
		/**
		 * Create a cube 3D box collider.
		 * 
		 * @param gameObject the gameObject object to which this collider has to be attached to.
		 * @param side the side of the cube.
		 */
		BoxCollider3D(GameObject gameObject, float side);
		/**
		 * Returns the bounding box of the collider, formed by the 8 vertices of the box.
		 * 
		 * @return the coordinates of the 8 vertices of the bounding box.
		 */
		array<vec3, 8> getBoundingBox();
		GameObject getGameObject();
		/**
		 * Check if the given point collides with this box collider.
		 * 
		 * @param point the point to check the collision with. 
		 * @return true if there is a collision, false otherwise.
		 */
		bool checkCollision(vec3 point);

		private:
		GameObject gameObject;
		float size_x, size_y, size_z;
		float x_low, x_high, y_low, y_high, z_low, z_high;
		array<vec3, 8> boundingBox;

		/**
		 * Create the bounding box for the collider, composed by the 8 vertices of the box.
		 * 
		 */
		void createBoundingBox();

	};
}

#endif