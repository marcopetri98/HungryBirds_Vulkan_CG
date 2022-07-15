#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "DummyRecursionSolver.hpp"
#include "Camera.hpp"
#include "GameObject.hpp"

using glm::mat4, glm::vec3, glm::vec4, glm::translate, glm::rotate, glm::radians, glm::normalize;

namespace graphics
{
	Camera::Camera(mat4 initialTransform)
	{
		this->initialTransform = initialTransform;
	}

	Camera::Camera(GameObject* followedObject, vec3 eyePos, vec3 vectorUp)
	{
		lookAtGameObject(followedObject, eyePos, vectorUp);
	}

	mat4 Camera::getInitialTransform()
	{
		return this->initialTransform;
	}

	mat4 Camera::getCurrentTransform()
	{
		return this->currentTransform;
	}

	vec3 Camera::getCurrentPos()
	{
		return vec3(this->currentTransform[3][0], this->currentTransform[3][1], this->currentTransform[3][2]);
	}

	vec3 Camera::getEyeObjDir() {
		return this->eyeObjDir;
	}

	vector<vec3> Camera::getMovingDirsFRBL() {
		return this->movingDirsFRBL;
	}

	void Camera::setCurrentTransform(mat4 transform)
	{
		this->currentTransform = transform;
	}

	void Camera::lookAtGameObject(GameObject* gameObject, vec3 eyePos, vec3 vectorUp)
	{
		this->currentTransform = glm::lookAt(eyePos, gameObject->getCurrentPos(), glm::normalize(vectorUp));
	}

	void Camera::setEyeObjDir(vec3 dir) {
		this->eyeObjDir = dir;
		this->movingDirsFRBL.resize(4);
		dir = normalize(dir);
		for (int i = 0; i < 4; i ++) {
			dir.y = 0;
			this->movingDirsFRBL[i] = dir;
			mat4 rotationMat = glm::rotate(mat4(1.0f), radians(-90.f), glm::vec3(0.0, 1.0, 0.0));
			dir = vec3(rotationMat * vec4(dir, 1.0));
		}
	}
}
