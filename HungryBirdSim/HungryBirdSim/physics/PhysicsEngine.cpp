#include "PhysicsEngine.hpp"
#include <algorithm>
#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "RayCast2D.hpp"
#include "RayCast3D.hpp"
#include "../graphics/engine/Collider.hpp"

using std::array, std::pair, std::find;
using glm::vec3, glm::mat4, glm::radians, glm::translate, glm::rotate, glm::scale, glm::inverse, glm::normalize, glm::length;
using graphics::Collider;

namespace physics {
	PhysicsEngine::PhysicsEngine(bool raycast3d, vec3 ambient_acc = vec3(0, -9.81f, 0), float collisionDamping=0.1) {
		this->raycast3d = raycast3d;
		this->ambient_acc = ambient_acc;
		this->collisionDamping = collisionDamping;
	}
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
		Collider* collider = gameobject.getCollider();
		CollisionInfo collision = CollisionInfo();
		RayCast* raycast;
		if (this->raycast3d) {
			//for now assume gameobject is only spherical and others are boxes
			RayCast3D raycastObj = RayCast3D(36.0f, (*collider).getSize(), 0.5f);
			raycast = &raycastObj;
		}
		else {
			RayCast2D raycastObj = RayCast2D(36.0f, vec3(0, 0, 1), (*collider).getSize(), 0.5f);
			raycast = &raycastObj;
		}
		while ((*raycast).hasNext() && !collision.collided) {
			pair<vec3, vec3> rayResult = (*raycast).nextRay(gameobject.getCurrentPos());
			for (GameObject go : others) {
				if ((*go.getCollider()).checkCollision(rayResult.second) && gameobject.getCollider()->getLastCollision()!=go.getName()) {
					collision = CollisionInfo(true, go, gameobject.getCurrentPos(), rayResult.first);
					gameobject.getCollider()->setLastCollision(go.getName());
					break;
				}
			}
		}
		return CollisionInfo();
	}
	
	void PhysicsEngine::track(GameObject gameobject) {
		vector<GameObject> go;
		go.push_back(gameobject);
		track(go);
	}

	void PhysicsEngine::track(vector<GameObject> gameobjects) {
		vector<string> names;
		for (GameObject go : this->trackedObjects) {
			names.push_back(go.getName());
		}
		for (int i = 0; i < gameobjects.size(); i++) {
			if (find(names.begin(), names.end(), gameobjects[i].getName()) == names.end()) {
				this->trackedObjects.push_back(gameobjects[i]);
			}
		}
	}

	void PhysicsEngine::untrack(GameObject gameobject) {
		vector<GameObject> go;
		go.push_back(gameobject);
		untrack(go);
	}

	void PhysicsEngine::untrack(vector<GameObject> gameobjects) {
		vector<string> names;
		for (GameObject go : gameobjects) {
			names.push_back(go.getName());
		}
		for (int i = 0; i < this->trackedObjects.size(); i++) {
			if (find(names.begin(), names.end(), this->trackedObjects[i].getName()) != names.end()) {
				this->trackedObjects.erase(this->trackedObjects.begin()+i);
			}
		}
	}

	void PhysicsEngine::untrackAll() {
		this->trackedObjects.clear();
	}

	void PhysicsEngine::update(float deltaTime) {
		for (int i = 0; i < this->trackedObjects.size(); i++) {
			GameObject go = this->trackedObjects[i];
			if (go.getVelocity() != vec3(0.f, 0.f, 0.f)) {
				vec3 velocity = go.getVelocity();
				go.setVelocity(velocity);
				vec3 movement = deltaTime * velocity;
				mat4 newPosition = translateObject(go, movement);
				vec3 currAcceleration = go.getAcceleration();
				currAcceleration = currAcceleration + this->ambient_acc;
				go.setAcceleration(currAcceleration);
				vector<GameObject> others = this->trackedObjects;
				others.erase(others.begin() + i);
				CollisionInfo collision = checkCollisions(go, others);
				if (collision.collided) {
					vec3 newVel = (normalize(velocity) + collision.collisionDir) * length(velocity) * this->collisionDamping;
					go.setVelocity(newVel);
				}
			}
		}
	}
}
