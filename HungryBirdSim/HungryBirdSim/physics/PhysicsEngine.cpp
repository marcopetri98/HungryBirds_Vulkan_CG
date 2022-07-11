#include "PhysicsEngine.hpp"
#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using std::array;
using glm::vec3, glm::mat4, glm::radians, glm::translate, glm::rotate, glm::scale, glm::inverse;

namespace physics {
	
	mat4 PhysicsEngine::translateObject(GameObject gameobject, vec3 translation) {
		return translate(gameobject.getCurrentTransform(), translation);
	}
	mat4 PhysicsEngine::rotateObject(GameObject gameobject, vec3 angles_xyz) {
		mat4 rotated = rotate(gameobject.getCurrentTransform(), radians(angles_xyz[0]), vec3(1, 0, 0));
		rotated = rotate(rotated, radians(angles_xyz[1]), vec3(0, 1, 0));
		rotated = rotate(rotated, radians(angles_xyz[2]), vec3(0, 0, 1));
		return rotated;
	}
	mat4 PhysicsEngine::rotateAroundAxis(GameObject gameobject, vec3 rotationPoint, float angle, vec3 alignedAxis, array<float, 2> alignmentAngles, array<vec3, 2> alignmentRotationAxis) {
		mat4 current = gameobject.getCurrentTransform();
		mat4 T = translate(mat4(1.0f), rotationPoint);
		mat4 R1 = rotate(mat4(1.0f), radians(alignmentAngles[0]), alignmentRotationAxis[0]);
		mat4 R2 = rotate(mat4(1.0f), radians(alignmentAngles[1]), alignmentRotationAxis[1]);
		mat4 R = rotate(mat4(1.0f), radians(angle), alignedAxis);

		return T * R2 * R1 * R * inverse(R1) * inverse(R2) * inverse(T) * current;
	}
	mat4 PhysicsEngine::scaleObject(GameObject gameobject, vec3 scales) {
		return scale(gameobject.getCurrentTransform(), scales);
	}

	void PhysicsEngine::translateObjectInPlace(GameObject gameobject, vec3 translation) {
		gameobject.setCurrentTransform(translateObject(gameobject, translation));
	}
	void PhysicsEngine::rotateObjectInPlace(GameObject gameobject, vec3 angles_xyz) {
		gameobject.setCurrentTransform(rotateObject(gameobject, angles_xyz));
	}
	void PhysicsEngine::rotateAroundAxisInPlace(GameObject gameobject, vec3 rotationPoint, float angle, vec3 alignedAxis, array<float, 2> alignmentAngles, array<vec3, 2> alignmentRotationAxis) {
		gameobject.setCurrentTransform(rotateAroundAxis(gameobject, rotationPoint, angle, alignedAxis, alignmentAngles, alignmentRotationAxis));
	}
	void PhysicsEngine::scaleObjectInPlace(GameObject gameobject, vec3 scales) {
		gameobject.setCurrentTransform(scaleObject(gameobject, scales));
	}
	CollisionInfo PhysicsEngine::checkCollisions(GameObject gameobject, vector<GameObject> others){
		return CollisionInfo();
	}
}
