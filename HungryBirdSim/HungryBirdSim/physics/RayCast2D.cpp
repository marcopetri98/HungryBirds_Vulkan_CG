#include <array>
#include <vector>
#include "RayCast2D.hpp"
#include "glm/glm.hpp"

using std::copy, std::begin, std::end, std::vector;

using glm::vec3, glm::radians;


namespace physics{
	RayCast2D::RayCast2D(float numRays, vec3 axis, float objRadius, float length) {
		this->numRays = numRays;
		this->axis = axis;
		this->length = length;
		this->objRadius = objRadius;
		this->currAngle = 0.f;
		this->anglePerStep = 360 / numRays;
	}

	vec3 RayCast2D::nextRay(vec3 pos) {
		float r = this->objRadius + this->length;
		vec3 rayDir;
		if (this->axis[0] == 1) {
			rayDir = glm::vec3(r * sin(radians(this->currAngle)), r * cos(radians(this->currAngle)), 0);
		}
		else if (this->axis[1] == 1) {
			rayDir = glm::vec3(0, r * sin(radians(this->currAngle)), r * cos(radians(this->currAngle)));
		}
		else {
			rayDir = glm::vec3(r * sin(radians(this->currAngle)), 0, r * cos(radians(this->currAngle)));
		}
		vec3 rayPos = rayDir + pos;
		this->currAngle += this->anglePerStep;
		return rayPos;
	}

	bool RayCast2D::hasNext() {
		return this->currAngle < 360;
	}
}
