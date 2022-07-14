#pragma once

#ifndef COLLIDER_HPP
#define COLLIDER_HPP

#include <string>
#include "DummyRecursionSolver.hpp"
#include "GameObject.hpp"

using glm::vec3;
using std::string;

namespace graphics
{
	class Collider
	{
		public:
		/**
		* Get the name of the last object that collided with this collider.
		*
		* @return The name of the last collided object.
		*/
		string getLastCollision();
		/**
		 * Set the name of the last object that collided with this collider.
		 *
		 * @param name The name of the last collided object.
		 */
		void setLastCollision(string name);
		/**
		 * Gets the game object that owns this collider.
		 * 
		 * @return The game object to which this collider is attached.
		 */
		virtual GameObject getGameObject() = 0;
		virtual void setGameObject(GameObject* gameObject) = 0;
		virtual float getSize() = 0;
		virtual bool checkCollision(vec3 point) = 0;

		private:
		string lastCollision;
	};
}

#endif