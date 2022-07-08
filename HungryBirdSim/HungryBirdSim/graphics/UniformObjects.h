#pragma once

#ifndef UNIFORMOBJECTS_H
#define UNIFORMOBJECTS_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <optional>
#include <glm/glm.hpp>

namespace graphics
{
	struct UniformBufferObject
	{
		alignas(16) glm::mat4 model;
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 proj;
	};
}

#endif