#include <string>

#include "Errors.h"

namespace errors
{
	string getErrorStr(Error type)
	{
		switch (type)
		{
			case Error::VULKAN_FAIL_CREATE_INSTANCE:
				return "Vulkan wasn't able to properly create the Vulkan instance (VkInstance)!";
				break;

			case Error::VULKAN_FAIL_VALIDATION_LAYERS:
				return "Validation layers have been requested. However, at least one of them is not available!";

			case Error::VULKAN_FAIL_DEBUG_MESSENGER:
				return "Debug messenger couldn't be set up, which means that the function has not been found!";
		}
	}
}
