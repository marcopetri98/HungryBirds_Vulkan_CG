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
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};
}

#endif