#include <array>
#include <vector>
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "RayCast3D.hpp"


using std::copy, std::begin, std::end, std::vector;

using glm::vec3, glm::vec4, glm::radians, glm::mat4, glm::rotate;


namespace physics {
	RayCast3D::RayCast3D(float numRaysPerDir, float objRadius, float length) {
		this->numRays = numRaysPerDir;
		this->length = length;
		this->objRadius = objRadius;
		this->currAngle = 0.f;
		this->anglePerStep = 360 / numRaysPerDir;
		this->currDir = 0;
	}

	vec3 RayCast3D::nextRay(vec3 pos) {
		float r = this->objRadius + this->length;
		vec3 rayDir;
		if (this->currDir == 0) {
			rayDir = glm::vec3(r * sin(radians(this->currAngle)), r * cos(radians(this->currAngle)), 0);
		}
		else if (this->currDir == 1) {
			rayDir = glm::vec3(0, r * sin(radians(this->currAngle)), r * cos(radians(this->currAngle)));
		}
		else if (this->currDir == 2) {
			rayDir = glm::vec3(r * sin(radians(this->currAngle)), 0, r * cos(radians(this->currAngle)));
		}
		else if (this->currDir == 3) {
			mat4 R = rotate(mat4(1.0f), radians(45.0f), vec3(0, 1, 0));
			rayDir = glm::vec3(r * sin(radians(this->currAngle)), 0, r * cos(radians(this->currAngle)));
			rayDir = R * vec4(rayDir, 0);
		}
		else {
			mat4 R = rotate(mat4(1.0f), radians(-45.0f), vec3(0, 1, 0));
			rayDir = glm::vec3(r * sin(radians(this->currAngle)), 0, r * cos(radians(this->currAngle)));
			rayDir = R * vec4(rayDir, 0);
		}
		vec3 rayPos = rayDir + pos;
		this->currAngle += this->anglePerStep;
		if (this->currAngle >= 360) {
			this->currAngle = 0.f;
			this->currDir += 1;
		}
		return rayPos;
	}

	bool RayCast3D::hasNext() {
		return this->currDir < 5;
	}
}
