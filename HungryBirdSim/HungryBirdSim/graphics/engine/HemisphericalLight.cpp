#include <glm/glm.hpp>

#include "HemisphericalLight.hpp"

using glm::vec3;

namespace graphics
{
	HemisphericalLight::HemisphericalLight(vec3 topColor, vec3 bottomColor, vec3 topDir, vec3 reflection)
	{
		this->topColor = topColor;
		this->bottomColor = bottomColor;
		this->topDir = topDir;
		this->reflection = reflection;
	}

	vec3 HemisphericalLight::getLightColor()
	{
		return this->topColor;
	}

	vec3 HemisphericalLight::getLightBottomColor()
	{
		return this->bottomColor;
	}

	vec3 HemisphericalLight::getLightTopDir()
	{
		return this->topDir;
	}

	vec3 HemisphericalLight::getLightReflection()
	{
		return this->reflection;
	}
}