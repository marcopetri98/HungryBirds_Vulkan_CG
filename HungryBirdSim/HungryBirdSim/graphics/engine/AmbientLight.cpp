#include <glm/glm.hpp>

#include "AmbientLight.hpp"

using glm::vec3;

namespace graphics
{
	AmbientLight::AmbientLight(vec3 color, vec3 reflection)
	{
		this->color = color;
		this->reflection = reflection;
	}

	vec3 AmbientLight::getLightColor()
	{
		return this->color;
	}

	vec3 AmbientLight::getLightReflection()
	{
		return this->reflection;
	}
}
