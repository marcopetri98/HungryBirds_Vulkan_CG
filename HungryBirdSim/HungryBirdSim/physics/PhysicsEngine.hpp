#pragma once

#ifndef PHYSICSENGINE_H_
#define PHYSICSENGINE_H_

#include <array>
#include <vector>
#include <glm/glm.hpp>

#include "../graphics/engine/DummyRecursionSolver.hpp"
#include "../graphics/engine/Object.hpp"
#include "CollisionInfo.hpp"
#include "RayCast.hpp"

using std::array, std::vector;
using glm::vec3, glm::mat4;
using graphics::Object;


namespace physics
{

	class PhysicsEngine {
		public:
		PhysicsEngine();
		PhysicsEngine(bool raycast3d, vec3 ambient_acc = vec3(0, -9.81f, 0), float collisionDamping = 0.3);
		/**
		 * Translate an object by the given vector of displacements.
		 * 
		 * @param gameobject The gameobject to translate.
		 * @param translation The vector of translation on the xyz axes.
		 * @return The translated transform matrix.
		 */
		mat4 translateObject(Object* gameobject, vec3 translation);
		/**
		 * Rotate an object around xyz axes with the given angles, the rotation order is x->y->z.
		 * 
		 * @param gameobject The gameobject to rotate.
		 * @param angles_xyz The vector of angles for the rotations on the xyz axes.
		 * @return The rotated transform matrix.
		 */
		mat4 rotateObject(Object* gameobject, vec3 angles_xyz);
		/**
		 * Rotate an object around an arbitrary axis passing through a point, given the angles to align one of the xyz axes with the desired rotation axis.
		 * 
		 * \param gameobject The gameobject to rotate.
		 * \param rotationPoint A point on the axis to which the gameobjec has to be rotated around.
		 * \param angle The rotation angle.
		 * \param alignedAxis The standard axis (xyz) which will be aligned with the arbitrary ones by means of the specified rotations.
		 * \param alignmentAngles The angles to perform rotations to align the specified standard axis (xyz) with the arbitrary axis, specified in the wanted order. 
		 * \param alignmentRotationAxis The standard axis (xyz) to perform rotations to align the arbitrary axis with the specified standard one.
		 * \return The rotated transform matrix.
		 */
		mat4 rotateAroundAxis(Object* gameobject, vec3 rotationPoint, float angle, vec3 alignedAxis, array<float, 2> alignmentAngles, array<vec3, 2> alignmentRotationAxis);
		/**
		 * Scale a gameobject.
		 * 
		 * \param gameobject The gameobject to scale.
		 * \param scales The scaling factors for each axis.
		 * \return The scaled transform matrix.
		 */
		mat4 scaleObject(Object* gameobject, vec3 scales);

		/**
		 * Wrapper for translateObject which operates in place.
		 */
		void translateObjectInPlace(Object* gameobject, vec3 translation);
		/**
		 * Wrapper for rotateObject which operates in place.
		 */
		void rotateObjectInPlace(Object* gameobject, vec3 angles_xyz);
		/**
		 * Wrapper for rotateAroundAxis which operates in place.
		 */
		void rotateAroundAxisInPlace(Object* gameobject, vec3 rotationPoint, float angle, vec3 alignedAxis, array<float, 2> alignmentAngles, array<vec3, 2> alignmentRotationAxis);
		/**
		 * Wrapper for scaleObject which operates in place.
		 */
		void scaleObjectInPlace(Object* gameobject, vec3 scales);
		/**
		 * Remove the object from the frustum.
		 * 
		 * @param gameobject The gameobject to hide.
		 */
		void hideObject(GameObject* gameobject);
		/**
		 * Check if the given object is colliding with one of the other gameobjects specified.
		 * 
		 * @param gameobject The gameobject to check the collisions.
		 * @param others Other gameobject to check collision with the specified one.
		 * @return a struct containing info about the collision. If there is no collision a CollisionInfo object is still returned, containing a false value in the 'collided' field.
		 */
		CollisionInfo checkCollisions(GameObject gameobject, vector<GameObject*> others);
		/**
		 * Start tracking a gameobject in order to update its position automatically when updates are called.
		 * 
		 * @param gameobject The gameobject to be tracked.
		 */
		void track(GameObject* gameobject);
		/**
		 * Start tracking all the specified gameobject in order to update their position automatically when updates are called.
		 *
		 * @param gameobjects The gameobjects to be tracked.
		 */
		void track(vector<GameObject*> gameobjects);
		/**
		 * Stop tracking a gameobject.
		 *
		 * @param gameobject The gameobject to be untracked.
		 */
		void untrack(GameObject* gameobject);
		/**
		 * Stop tracking all the specified gameobjects.
		 *
		 * @param gameobjects The gameobjects to be untracked.
		 */
		void untrack(vector<GameObject*> gameobjects);
		/**
		 * Clear all the tracked gameobjects.
		 * 
		 */
		void untrackAll();
		/**
		 * Updates the physics for all the tracked objects.
		 * 
		 */
		void update(float deltaTime);

		private:
		vector<GameObject*> trackedObjects;
		bool raycast3d;
		vec3 ambient_acc;
		float collisionDamping;
		int collisionTags;
		int movableTag;
		int groundTags;
		int enemyTags;
		int collectablesTags;
	};
}

#endif