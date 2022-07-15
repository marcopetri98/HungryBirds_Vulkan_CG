#include "PhysicsEngine.hpp"
#include <algorithm>
#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "RayCast2D.hpp"
#include "RayCast3D.hpp"
#include "../graphics/engine/Collider.hpp"
#include "../graphics/engine/Tags.hpp"
#include "../graphics/engine/Object.hpp"

using std::array, std::pair, std::find;
using glm::vec3, glm::mat4, glm::radians, glm::translate, glm::rotate, glm::scale, glm::inverse, glm::normalize, glm::length;
using graphics::Collider, graphics::Object;
using tags::Tag, tags::getTags;

namespace physics {
	PhysicsEngine::PhysicsEngine()
	{

	}

	PhysicsEngine::PhysicsEngine(bool raycast3d, vec3 ambient_acc, float collisionDamping) {
		this->raycast3d = raycast3d;
		this->ambient_acc = ambient_acc;
		this->collisionDamping = collisionDamping;

		vector<Tag> cTags{Tag::MOVABLE_COLLIDABLE_OBJECT, Tag::RIGID_COLLIDABLE_OBJECT};
		this->collisionTags = getTags(cTags);

		vector<Tag> gTags{ Tag::GROUND};
		this->groundTags = getTags(gTags);

	}
	mat4 PhysicsEngine::translateObject(Object* gameobject, vec3 translation) {
		mat4 T = translate(mat4(1.0f), translation);
		return T * gameobject->getCurrentTransform();
	}
	mat4 PhysicsEngine::rotateObject(Object* gameobject, vec3 angles_xyz) {
		mat4 R1 = rotate(mat4(1.0f), radians(angles_xyz[0]), vec3(1, 0, 0));
		mat4 R2 = rotate(mat4(1.0f), radians(angles_xyz[1]), vec3(0, 1, 0));
		mat4 R3 = rotate(mat4(1.0f), radians(angles_xyz[2]), vec3(0, 0, 1));
		return R3 * R2 * R1 * gameobject->getCurrentTransform();
	}
	mat4 PhysicsEngine::rotateAroundAxis(Object* gameobject, vec3 rotationPoint, float angle, vec3 alignedAxis, array<float, 2> alignmentAngles, array<vec3, 2> alignmentRotationAxis) {
		mat4 current = gameobject->getCurrentTransform();
		mat4 T = translate(mat4(1.0f), rotationPoint);
		mat4 R1 = rotate(mat4(1.0f), radians(alignmentAngles[0]), alignmentRotationAxis[0]);
		mat4 R2 = rotate(mat4(1.0f), radians(alignmentAngles[1]), alignmentRotationAxis[1]);
		mat4 R = rotate(mat4(1.0f), radians(angle), alignedAxis);

		return T * R2 * R1 * R * inverse(R1) * inverse(R2) * inverse(T) * current;
	}
	mat4 PhysicsEngine::scaleObject(Object* gameobject, vec3 scales) {
		mat4 S = scale(mat4(1.0f), scales);
		return S * gameobject->getCurrentTransform();
	}

	void PhysicsEngine::translateObjectInPlace(Object* gameobject, vec3 translation) {
		gameobject->setCurrentTransform(translateObject(gameobject, translation));
	}
	void PhysicsEngine::rotateObjectInPlace(Object* gameobject, vec3 angles_xyz) {
		gameobject->setCurrentTransform(rotateObject(gameobject, angles_xyz));
	}
	void PhysicsEngine::rotateAroundAxisInPlace(Object* gameobject, vec3 rotationPoint, float angle, vec3 alignedAxis, array<float, 2> alignmentAngles, array<vec3, 2> alignmentRotationAxis) {
		gameobject->setCurrentTransform(rotateAroundAxis(gameobject, rotationPoint, angle, alignedAxis, alignmentAngles, alignmentRotationAxis));
	}
	void PhysicsEngine::scaleObjectInPlace(Object* gameobject, vec3 scales) {
		gameobject->setCurrentTransform(scaleObject(gameobject, scales));
	}
	CollisionInfo PhysicsEngine::checkCollisions(GameObject gameobject, vector<GameObject*> others){
		Collider* collider = gameobject.getCollider();
		CollisionInfo collision = CollisionInfo();
		RayCast* raycast;
		if (this->raycast3d) {
			//for now assume gameobject is only spherical and others are boxes
			RayCast3D raycastObj = RayCast3D(360.0f, (*collider).getSize(), 0.3f);
			raycast = &raycastObj;
		}
		else {
			RayCast2D raycastObj = RayCast2D(36.0f, vec3(0, 0, 1), (*collider).getSize(), 0.5f);
			raycast = &raycastObj;
		}
		while ((*raycast).hasNext() && !collision.collided) {
			pair<vec3, vec3> rayResult = (*raycast).nextRay(gameobject.getCurrentPos());
			for (GameObject* go : others) {
				if (go->getCollider()->checkCollision(rayResult.second) && gameobject.getCollider()->getLastCollision() != go->getName()) {
					collision = CollisionInfo(true, *go, gameobject.getCurrentPos(), rayResult.first);
					gameobject.getCollider()->setLastCollision(go->getName());
					break;
				}
			}
		}
		return collision;
	}
	
	void PhysicsEngine::track(GameObject* gameobject) {
		vector<GameObject*> go;
		go.push_back(gameobject);
		track(go);
	}

	void PhysicsEngine::track(vector<GameObject*> gameobjects) {
		vector<string> names;
		for (GameObject* go : this->trackedObjects) {
			names.push_back(go->getName());
		}
		for (int i = 0; i < gameobjects.size(); i++) {
			if (find(names.begin(), names.end(), gameobjects[i]->getName()) == names.end()) {
				this->trackedObjects.push_back(gameobjects[i]);
			}
		}
	}

	void PhysicsEngine::untrack(GameObject* gameobject) {
		vector<GameObject*> go;
		go.push_back(gameobject);
		untrack(go);
	}

	void PhysicsEngine::untrack(vector<GameObject*> gameobjects) {
		vector<string> names;
		for (GameObject* go : gameobjects) {
			names.push_back(go->getName());
		}
		for (int i = 0; i < this->trackedObjects.size(); i++) {
			if (find(names.begin(), names.end(), this->trackedObjects[i]->getName()) != names.end()) {
				this->trackedObjects.erase(this->trackedObjects.begin()+i);
			}
		}
	}

	void PhysicsEngine::untrackAll() {
		this->trackedObjects.clear();
	}

	void PhysicsEngine::update(float deltaTime) {
		for (int i = 0; i < this->trackedObjects.size(); i++) {
			GameObject* go = this->trackedObjects[i];
			if (go->getVelocity() != vec3(0.f, 0.f, 0.f)) {
				vec3 velocity = go->getVelocity();
				vec3 movement = deltaTime * velocity;
				translateObjectInPlace(go, movement);
				vec3 currAcceleration = go->getAcceleration();
				currAcceleration = this->ambient_acc;
				go->setAcceleration(currAcceleration);
				velocity += currAcceleration * deltaTime;
				go->setVelocity(velocity);
				vector<GameObject*> others;
				for (int j = 0; j < this->trackedObjects.size(); j++) {
					if (i != j) {
						others.push_back(this->trackedObjects[j]);
					}
				}
				CollisionInfo collision = checkCollisions(*go, others);
				if (collision.collided) {
					if (collision.collidedObject.getTag() & this->collisionTags) {
						vec3 newVel = (normalize(velocity) - collision.collisionDir) * length(velocity) * (1-this->collisionDamping);
						go->setVelocity(newVel);
					}
					else if ((collision.collidedObject.getTag() & this->groundTags)) {
						go->setVelocity(vec3(0, 0, 0));
						go->setAcceleration(vec3(0, 0, 0));
					}
				}
			}
		}
	}
}
