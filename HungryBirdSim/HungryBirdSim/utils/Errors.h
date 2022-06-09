#pragma once

#ifndef ERRORS_H_
#define ERRORS_H_

#include <string>

using std::string;

namespace errors
{
	enum class Error
	{
		VULKAN_FAIL_CREATE_INSTANCE,
		VULKAN_FAIL_VALIDATION_LAYERS,
		VULKAN_FAIL_DEBUG_MESSENGER
	};

	string getErrorStr(Error type);
}

#endif
