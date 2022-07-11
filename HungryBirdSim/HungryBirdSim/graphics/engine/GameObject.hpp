#pragma once

#ifndef GAMEOBJECT_HPP
#define GAMEOBJECT_HPP

#include <glm/glm.hpp>

#include <string>

#include "DummyRecursionSolver.hpp"
#include "Collider.hpp"

using std::string;
using glm::vec3;
using glm::mat4;

namespace graphics
{
	class GameObject
	{
		public:
		/**
		 * Creates a game object.
		 * 
		 * @param name The unique identifier of the object.
		 * @param tag The tag of the object.
		 * @param objectPath The path to the object file of the object.
		 * @param texturePath The path to the texture of the object.
		 * @param initialTransform The initial transform of the object expressed as matrix-on-the-left.
		 * @param layer The layer of the object.
		 */
		GameObject(string name = "none", int tag = 0, string objectPath = "none", string texturePath = "none", mat4 initialTransform = mat4(1), int layer = 0);
		/**
		 * Gets the name of the object.
		 * 
		 * @return Unique identifier of the object.
		 */
		string getName();
		/**
		 * Gets the tag of the object.
		 * 
		 * @return Tag of the object.
		 */
		int getTag();
		/**
		 * Gets the layer of the object.
		 * 
		 * @return Layer of the object.
		 */
		int getLayer();
		/**
		 * Gets the initial rotation of the object expressed as (degrees on x, degrees on y, degrees on z).
		 * 
		 * @return Initial rotation of the object.
		 */
		mat4 getInitialTransform();
		/**
		 * Gets the current rotation.
		 * 
		 * @return Current rotation of the object.
		 */
		mat4 getCurrentTransform();
		/**
		 * Gets the current position of the object.
		 * 
		 * @return Position of the object specified as xyz. 
		 */
		vec3 getCurrentPos();
		/**
		 * Gets the string representing the path to the object file.
		 * 
		 * @return Path to object file.
		 */
		string getObjectPath();
		/**
		 * Gets the string representing the path to the texture file.
		 * 
		 * @return Path to texture file.
		 */
		string getTexturePath();
		/**
		 * Gets the pointer to the collider.
		 * 
		 * @return Pointer to collider of the object
		 */
		Collider* getCollider();
		/**
		 * Changes the current rotation of the object expressed as matrix-on-the-left.
		 * 
		 * @param rot New rotation of the object expressed as matrix-on-the-left.
		 */
		void setCurrentTransform(mat4 transform);
		/**
		 * Sets the collider of this GameObject.
		 * 
		 * @param collider Pointer to the collider of this object.
		 */
		void setCollider(Collider* collider);

		private:
		/**
		 * The unique name identifier for the GameObject.
		 */
		string name;
		/**
		 * The tag of the GameObject.
		 */
		int tag;
		/**
		 * The layer of the GameObject.
		 */
		int layer;
		/**
		 * The initial rotation of the GameObject expressed as matrix-on-the-left.
		 */
		mat4 initialTransform;
		/**
		 * The current rotation of the GameObject expressed as matrix-on-the-left.
		 */
		mat4 currentTransform;
		/**
		 * The path of the obj file identifying the object structure.
		 */
		string objectPath;
		/**
		 * The path to the texture of the object.
		 */
		string texturePath;
		/**
		 * The collider of the object.
		 */
		Collider* collider;
	};
}

#endif