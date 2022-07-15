#pragma once

#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <glm/glm.hpp>

using glm::mat4;
using glm::vec3;

namespace graphics
{
	class Object
	{
		public:
		virtual mat4 getCurrentTransform() = 0;
		virtual vec3 getCurrentPos() = 0;
		virtual void setCurrentTransform(mat4 transform) = 0;
	};
}

#endif