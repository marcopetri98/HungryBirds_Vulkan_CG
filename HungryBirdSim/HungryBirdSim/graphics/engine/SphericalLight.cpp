#include <glm/glm.hpp>

#include "SphericalLight.hpp"

using glm::vec3;

namespace graphics
{
	SphericalLight::SphericalLight(vec3 colorDeviationX, vec3 colorDeviationY, vec3 colorDeviationZ, vec3 reflection)
	{
		this->colorDeviationX = colorDeviationX;
		this->colorDeviationY = colorDeviationY;
		this->colorDeviationZ = colorDeviationZ;
		this->reflection = reflection;
	}

	vec3 SphericalLight::getLightColor()
	{
		return this->color;
	}

	vec3 SphericalLight::getLightDeviationXColor()
	{
		return this->colorDeviationX;
	}

	vec3 SphericalLight::getLightDeviationYColor()
	{
		return this->colorDeviationY;
	}

	vec3 SphericalLight::getLightDeviationZColor()
	{
		return this->colorDeviationZ;
	}

	vec3 SphericalLight::getLightReflection()
	{
		return this->reflection;
	}
}