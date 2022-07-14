#include <glm/glm.hpp>

#include "SpotLight.hpp"

using glm::vec3;

namespace graphics
{
	SpotLight::SpotLight(vec3 direction, vec3 position, vec3 color, float distanceReduction, float decay, float cosineOuter, float cosineInner)
	{
		this->direction = direction;
		this->position = position;
		this->color = color;
		this->distanceReduction = distanceReduction;
		this->decay = decay;
		this->cosineOuter = cosineOuter;
		this->cosineInner = cosineInner;
	}

	vec3 SpotLight::getLightDir()
	{
		return this->direction;
	}

	vec3 SpotLight::getLightPos()
	{
		return this->position;
	}

	vec3 SpotLight::getLightColor()
	{
		return this->color;
	}

	float SpotLight::getDistanceReduction()
	{
		return this->distanceReduction;
	}

	float SpotLight::getDecay()
	{
		return this->decay;
	}

	float SpotLight::getCosineOuterAngle()
	{
		return this->cosineOuter;
	}

	float SpotLight::getCosineInnerAngle()
	{
		return this->cosineInner;
	}
}
