#pragma once

#ifndef GRAPHICSENGINE_H_
#define GRAPHICSENGINE_H_
#define GLFW_INCLUDE_VULKAN
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_EXPOSE_NATIVE_WIN32

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <string>
#include <vector>

#include "Queues.h"
#include "SwapChainSupportDetails.h"




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
		VkDevice device;
		VkQueue graphicsQueue;
		VkQueue presentQueue;
		VkSurfaceKHR surface;
		VkSwapchainKHR swapChain;
		VkFormat swapChainImageFormat;
		VkExtent2D swapChainExtent;

		std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};
		std::vector<VkImage> swapChainImages;
		std::vector<VkImageView> swapChainImageViews;

		void initWindow();
		void initVulkan();
		void createInstance();
		vector<const char*> getRequiredExtensions();
		void checkSupportedExtensions();
		void printAvailableExtensions(vector<VkExtensionProperties> extensions);
		void checkGLFWRequiredExtensions(vector<VkExtensionProperties> extensions);
		/**
		 * Checks if all the requested validation layers are supported.
		 * 
		 * @return `true` if all the requested validation layers are present, `false` otherwise.
		 */
		bool checkValidationLayerSupport();
		/**
		 * Setup the debug messenger to be used with validation layers
		 * 
		 * @throw runtime_error if the creation of the debug utils wasn't successful
		 * @see CreateDebugUtilsMessengerEXT
		 */
		void setupDebugMessenger();
		/**
		 * Builds the struct to create the debug messenger.
		 * 
		 * @param createInfo the reference to the create info to fill
		 */
		void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
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
		 * Creates a logical device to interface with the selected physical device and retrieves its queries.
		 */
		void createLogicalDevice();
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
		/**
		 * Creates the surface used to present rendered images, handling different OS types automatically. 
		 * 
		 */
		void createSurface();
		/**
		 * Checks if all the required extensions are supported by a device
		 * 
		 * @param device is the device to be checked
		 * @return True if the device supports all the required extensions
		 */
		bool checkDeviceExtensionSupport(VkPhysicalDevice device);
		/**
		 * Checks if the device supports swap chain related requirements
		 * 
		 * @param device is the device to be checked
		 * @return True if the device supports all the required swap chain properties
		 */
		SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
		/**
		 * Chooses the best surface format supported (color depth)
		 * 
		 * @param availableFormats the available formats
		 * @return the chosen surface format
		 */
		VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		/**
		 * Chooses the presentation format. If MailboxKHR is supported is chosen, else FifoKHR is used.
		 *
		 * @param availableFormats the available formats
		 * @return the chosen surface format
		 */
		VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		//TODO we may want to choose which presentation format we want or which one we prefer in order

		/**
		 * Chooses the swap extent to match the previously created window
		 * 
		 * @param capabilities the available surface capabilities
		 * @return the chosen extent
		 */
		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
		/**
		 * Creates the swap chain
		 */
		void createSwapChain();
		/**
		 * Create the image views
		 */
		void createImageViews();
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