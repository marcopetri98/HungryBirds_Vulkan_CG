#include <glm/glm.hpp>

#include "DirectionalLight.hpp"

using glm::vec3;

namespace graphics
{
	DirectionalLight::DirectionalLight(vec3 direction, vec3 color)
	{
		this->direction = direction;
		this->color = color;
	}

	vec3 DirectionalLight::getLightDir()
	{
		return this->direction;
	}

	vec3 DirectionalLight::getLightColor()
	{
		return this->color;
	}
}
