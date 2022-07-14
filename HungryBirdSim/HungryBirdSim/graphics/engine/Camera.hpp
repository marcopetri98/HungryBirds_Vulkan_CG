#pragma once

#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>

#include "DummyRecursionSolver.hpp"
#include "GameObject.hpp"

using glm::mat4;
using glm::vec3;

namespace graphics
{
	class Camera
	{
		public:
		/**
		 * Creates the camera.
		 * 
		 * @param initialTransform The initial transformation of the camera.
		 */
		Camera(mat4 initialTransform = mat4(1));
		/**
		 * Creates the camera.
		 * 
		 * @param followedObject The GameObject to be watched.
		 * @param eyePos The position of the eyes of the camera.
		 * @param vectorUp The direction of the up vector.
		 */
		Camera(GameObject* followedObject, vec3 eyePos, vec3 vectorUp);
		/**
		 * Gets the initial transformation of the camera.
		 * 
		 * @return Initial transform of the camera.
		 */
		mat4 getInitialTransform();
		/**
		 * Get current transformation of the camera.
		 * 
		 * @return Current transform of the camera.
		 */
		mat4 getCurrentTransform();
		/**
		 * Get the current eye position of the camera.
		 * 
		 * @return Get the current position of the camera.
		 */
		vec3 getCurrentPos();
		/**
		 * Makes the camera to look at one object.
		 * 
		 * @param gameObject The GameObject to be watched.
		 * @param eyePos The position of the eyes of the camera.
		 * @param vectorUp The direction of the up vector.
		 */
		void lookAtGameObject(GameObject* gameObject, vec3 eyePos, vec3 vectorUp);
		/**
		 * Set the current tranform of the camera.
		 * 
		 * @param transform The new transform of the camera.
		 */
		void setCurrentTransform(mat4 transform);

		private:
		/**
		 * The initial transform of the camera.
		 */
		mat4 initialTransform;
		/**
		 * The current transform of the camera.
		 */
		mat4 currentTransform;
	};
}

#endif