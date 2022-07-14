#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "DummyRecursionSolver.hpp"
#include "Camera.hpp"
#include "GameObject.hpp"

using glm::mat4;

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
		return vec3(this->currentTransform[0][3], this->currentTransform[1][3], this->currentTransform[2][3]);
	}

	void Camera::setCurrentTransform(mat4 transform)
	{
		this->currentTransform = transform;
	}

	void Camera::lookAtGameObject(GameObject* gameObject, vec3 eyePos, vec3 vectorUp)
	{
		this->currentTransform = glm::lookAt(eyePos, gameObject->getCurrentPos(), glm::normalize(vectorUp));
	}
}
