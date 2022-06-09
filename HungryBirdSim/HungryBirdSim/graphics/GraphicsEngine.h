#pragma once

#ifndef GRAPHICSENGINE_H_
#define GRAPHICSENGINE_H_

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>
#include <vector>

#include "Queues.h"

using std::string;
using std::vector;

namespace graphics
{
	const string DEFAULT_TITLE = "Vulkan application";
	const int DEFAULT_WIDTH = 1200;
	const int DEFAULT_HEIGHT = 600;
	const vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };

	class GraphicsEngine
	{
		public:
		GraphicsEngine(string title = DEFAULT_TITLE, int width = DEFAULT_WIDTH, int height = DEFAULT_HEIGHT);
		void run();
		void setUseValidationLayers(bool val);

		private:
		string title;
		int width;
		int height;
		bool useValidationLayers;
		GLFWwindow* window;
		VkInstance instance;
		VkDebugUtilsMessengerEXT debugMessenger;
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

		void initWindow();
		void initVulkan();
		void createInstance();
		vector<const char*> getRequiredExtensions();
		void checkSupportedExtensions();
		void printAvailableExtensions(vector<VkExtensionProperties> extensions);
		void checkGLFWRequiredExtensions(vector<VkExtensionProperties> extensions);
		bool checkValidationLayerSupport();
		void setupDebugMessenger();
		void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		void pickPhysicalDevice();
		bool isDeviceSuitable(VkPhysicalDevice device);
		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
		void mainLoop();
		void cleanup();

		static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
													 const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
													 const VkAllocationCallbacks* pAllocator,
													 VkDebugUtilsMessengerEXT* pDebugMessenger);
		static void DestroyDebugUtilsMessengerEXT(VkInstance instance,
												  VkDebugUtilsMessengerEXT debugMessenger,
												  const VkAllocationCallbacks* pAllocator);
		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
															VkDebugUtilsMessageTypeFlagsEXT messageType,
															const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
															void* pUserData);
	};
}

#endif