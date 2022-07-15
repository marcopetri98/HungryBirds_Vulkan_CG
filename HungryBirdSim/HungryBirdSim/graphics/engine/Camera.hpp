#pragma once

#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>

#include "DummyRecursionSolver.hpp"
#include "GameObject.hpp"
#include "Object.hpp"

using glm::mat4;
using glm::vec3;

namespace graphics
{
	class Camera : public Object
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
		 * Get the current eye direction of the camera wrt the looked object.
		 *
		 * @return Get the current watch direction.
		 */
		vec3 getEyeObjDir();

		vector<vec3> getMovingDirsFRBL();
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
		/**
		 * Set the current eyedir.
		 * 
		 * @param dir The direction of the eye wrt the looked object.
		 */
		void setEyeObjDir(vec3 dir);

		private:
		/**
		 * The initial transform of the camera.
		 */
		mat4 initialTransform;
		/**
		 * The current transform of the camera.
		 */
		mat4 currentTransform;
		/**
		 * The direction vector pointing at the looked object.
		 */
		vec3 eyeObjDir;
		/**
		 * Vector containing the moving directions, ordered as: FORWARD, RIGHT, BACKWARD, LEFT.
		 */
		vector<vec3> movingDirsFRBL;
	};
}

#endif