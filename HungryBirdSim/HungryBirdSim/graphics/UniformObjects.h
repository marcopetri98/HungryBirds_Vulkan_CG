#pragma once

#ifndef UNIFORMOBJECTS_H
#define UNIFORMOBJECTS_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <optional>
#include <glm/glm.hpp>

using glm::mat4;
using glm::vec3, glm::vec1, glm::vec2;

namespace graphics
{
	struct GlobalUniformBufferObject
	{
		alignas(16) mat4 view;
		alignas(16) mat4 proj;
	};

	struct GlobalUniformBufferObjectLight
	{
		alignas(16) vec3 eyePos;
		alignas(16) vec3 directionalDir;
		alignas(16) vec3 directionalColor;
		alignas(16) vec3 pointColor;
		alignas(16) vec3 pointPos;
		alignas(16) vec3 spotDir;
		alignas(16) vec3 spotColor;
		alignas(16) vec3 spotPos;
		alignas(16) vec3 ambientColor;
		alignas(16) vec3 ambientReflection;
		alignas(16) vec3 hemisphericalTopColor;
		alignas(16) vec3 hemisphericalBottomColor;
		alignas(16) vec3 hemisphericalTopDir;
		alignas(16) vec3 hemisphericalReflection;
		alignas(16) vec3 sphericalColor;
		alignas(16) vec3 sphericalColorDeviationX;
		alignas(16) vec3 sphericalColorDeviationY;
		alignas(16) vec3 sphericalColorDeviationZ;
		alignas(16) vec3 sphericalReflection;
		alignas(16) vec3 selectorDiffuse;
		alignas(16) vec3 selectorSpecular;
		alignas(4) float pointDecay;
		alignas(4) float pointDistanceReduction;
		alignas(4) float spotDecay;
		alignas(4) float spotDistanceReduction;
		alignas(4) float spotCosineOuterAngle;
		alignas(4) float spotCosineInnerAngle;
		alignas(4) int selectorDirectional;
		alignas(4) int selectorAmbient;
		alignas(4) int selectorHemispherical;
		alignas(4) int selectorSpherical;
		alignas(4) int selectorPoint;
		alignas(4) int selectorSpot;
	};

	struct UniformBufferObject
	{
		alignas(16) mat4 modelVertices;
		alignas(16) mat4 modelNormal;
	};

	struct UniformBufferObjectLight
	{
		alignas(4) int selectorBackground;
		alignas(4) int selectorObject;
	};
}

#endif