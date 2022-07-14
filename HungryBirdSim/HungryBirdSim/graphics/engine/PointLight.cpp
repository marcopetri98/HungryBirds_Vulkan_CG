#include <glm/glm.hpp>

#include "PointLight.hpp"

using glm::vec3;

namespace graphics
{
	PointLight::PointLight(vec3 position, vec3 color, float distanceReduction, float decay)
	{
		this->position = position;
		this->color = color;
		this->decay = decay;
		this->distanceReduction = distanceReduction;
	}

	vec3 PointLight::getLightPos()
	{
		return this->position;
	}

	vec3 PointLight::getLightColor()
	{
		return this->color;
	}

	float PointLight::getDecay()
	{
		return this->decay;
	}

	float PointLight::getDistanceReduction()
	{
		return this->distanceReduction;
	}
}
