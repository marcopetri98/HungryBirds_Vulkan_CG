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
		// TODO: insert a parametrized way to handle required queues by using attributes
		// TODO: insert a parametrized way to choose which are the requirements for a GPU to be suitable
		// TODO: insert a parametrized wat to manage scoring of GPUs
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
		/**
		 * Rates all the available devices and choose the best one.
		 * 
		 * For each rated device, we control if it is suitable for the purposes
		 * of the application. In case it passes suitability test, it is scored
		 * and inserted in a ranking.
		 * 
		 * @throw runtime_error if there aren't GPUs or if there isn't a suitable one.
		 * 
		 */
		void pickPhysicalDevice();
		/**
		 * Decides if a device is suitable for the application.
		 * 
		 * @param device the physical device to be analysed for requirements of the application.
		 * @return `true` if the device is suitable for application purposes, `false` otherwise.
		 */
		bool isDeviceSuitable(VkPhysicalDevice device);
		/**
		 * Gives a suitability score to the device.
		 * 
		 * @param device is the device to be scored
		 * @return suitability score
		 */
		int rateDeviceSuitability(VkPhysicalDevice device);
		/**
		 * Searches for the desired queues in the given device.
		 * 
		 * @param device the device on which we want to search the required queues
		 * @return the index of the found GPU
		 */
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