#pragma once

#ifndef GAMEOBJECT_HPP
#define GAMEOBJECT_HPP

#include <glm/glm.hpp>

#include <string>

#include "DummyRecursionSolver.hpp"
#include "Collider.hpp"

using std::string;
using glm::vec3;

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
		 * @param initialPos The initial position of the object.
		 * @param initialRot The initial rotation of the object expressed as (degrees on x, degrees on y, degrees on z).
		 * @param layer The layer of the object.
		 */
		GameObject(string name = "none", int tag = 0, string objectPath = "none", string texturePath = "none", vec3 initialPos = vec3(1), vec3 initialRot = vec3(1), int layer = 0);
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
		 * Gets the initial position of the object.
		 * 
		 * @return Initial position of the object.
		 */
		vec3 getInitialPos();
		/**
		 * Gets the initial rotation of the object expressed as (degrees on x, degrees on y, degrees on z).
		 * 
		 * @return Initial rotation of the object.
		 */
		vec3 getInitialRot();
		/**
		 * Gets the current position.
		 * 
		 * @return Current position of the object.
		 */
		vec3 getCurrentPos();
		/**
		 * Gets the current rotation.
		 * 
		 * @return Current rotation of the object.
		 */
		vec3 getCurrentRot();
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
		 * Chnges the current position of the object.
		 * 
		 * @param pos The new position of the object.
		 */
		void setCurrentPos(vec3 pos);
		/**
		 * Changes the current rotation of the object expressed as (degrees on x, degrees on y, degrees on z).
		 * 
		 * @param rot New rotation of the object expressed as (degrees on x, degrees on y, degrees on z).
		 */
		void setCurrentRot(vec3 rot);
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
		 * The initial position of the GameObject.
		 */
		vec3 initialPos;
		/**
		 * The initial rotation of the GameObject expressed as (degrees on x, degrees on y, degrees on z).
		 */
		vec3 initialRot;
		/**
		 * The current position of the GameObject.
		 */
		vec3 currentPos;
		/**
		 * The current rotation of the GameObject expressed as (degrees on x, degrees on y, degrees on z).
		 */
		vec3 currentRot;
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