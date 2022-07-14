#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#ifndef STB_IMAGE_IMPLEMENTATION
	#define STB_IMAGE_IMPLEMENTATION
	#include <stb_image.h>
#endif

#ifndef TINYOBJLOADER_IMPLEMENTATION
	#define TINYOBJLOADER_IMPLEMENTATION
	#include <tiny_obj_loader.h>
#endif

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm> // Necessary for std::clamp
#include <chrono>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <limits>
#include <array>
#include <set>
#include <map>
#include <unordered_map>

#include "DummyRecursionSolver.hpp"
#include "GraphicsEngine.h"
#include "SceneLoader.hpp"
#include "../utils/Errors.h"
#include "../utils/CollectionUtils.hpp"

#define VULKANUTILS_IMPLEMENTATION
#include "../utils/VulkanUtils.hpp"

using std::vector;
using std::cout;
using std::endl;
using errors::getErrorStr;
using errors::Error;
using collectionutils::vectorContains;

namespace graphics
{
	GraphicsEngine::GraphicsEngine(string title, int width, int height, float nearPlane, float farPlane)
	{
		this->title = title;
		this->width = width;
		this->height = height;
		this->activeScene = NULL;
		this->nearPlane = nearPlane;
		this->farPlane = farPlane;
		useValidationLayers = true;
	}
	
	void GraphicsEngine::run()
	{
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}

	void GraphicsEngine::setUseValidationLayers(bool val)
	{
		useValidationLayers = val;
	}

	void GraphicsEngine::setPhysicsEngine(PhysicsEngine* engine)
	{
		this->physicsEngine = engine;
	}

	void GraphicsEngine::addScenes(vector<Scene*> scenes)
	{
		for (Scene* scene : scenes)
		{
			if (vectorContains(this->sceneNames, scene->getName()))
			{
				throw std::runtime_error("A scene with name " + scene->getName() + " already exists!");
			}
			else if (vectorContains(this->sceneIds, scene->getId()))
			{
				throw std::runtime_error("A scene with id " + std::to_string(scene->getId()) + " already exists!");
			}
			else
			{
				this->allScenes.push_back(scene);
				this->sceneNames.push_back(scene->getName());
				this->sceneIds.push_back(scene->getId());
				this->mapSceneNamesIds.insert(std::pair<string, int>(scene->getName(), scene->getId()));
				this->mapSceneIdsToPos.insert(std::pair<int, int>(scene->getId(), this->allScenes.size() - 1));
			}
		}
	}

	void GraphicsEngine::selectScene(string sceneName)
	{
		if (!vectorContains(this->sceneNames, sceneName))
		{
			throw std::runtime_error("There isn't a scene with name " + sceneName);
		}
		else
		{
			selectScene(this->mapSceneNamesIds[sceneName]);
		}
	}

	void GraphicsEngine::selectScene(int sceneId)
	{
		if (!vectorContains(this->sceneIds, sceneId))
		{
			throw std::runtime_error("There isn't a scene with id " + std::to_string(sceneId));
		}
		else
		{
			if (activeScene == NULL)
			{
				activeScene = allScenes[this->mapSceneIdsToPos[sceneId]];
				this->updatePhysicsEngine();
				sceneLoader = SceneLoader(this, MAX_FRAMES_IN_FLIGHT, *activeScene);
			}
			else
			{
				activeScene = allScenes[this->mapSceneIdsToPos[sceneId]];
				this->updatePhysicsEngine();
				sceneLoader.cleanup();
				sceneLoader = SceneLoader(this, MAX_FRAMES_IN_FLIGHT, *activeScene);
				sceneLoader.createDescriptorPoolsAndObjects();
			}
		}
	}

	void GraphicsEngine::updatePhysicsEngine()
	{
		vector<GameObject*> gameObjectPointers;

		gameObjectPointers.clear();
		for (int i = 0; i < activeScene->getNumOfGameObjects(); i++)
		{
			gameObjectPointers.push_back(activeScene->getGameObjectPointerByPos(i));
		}

		physicsEngine->track(gameObjectPointers);
	}

	void GraphicsEngine::initWindow()
	{
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	}

	void GraphicsEngine::initVulkan()
	{
		// instance creation
		createInstance();
		setupDebugMessenger();
		createSurface();

		// device creation
		pickPhysicalDevice();
		createLogicalDevice();

		// creation of the swap chain
		createSwapChain();
		createImageViews();

		// creation of command pools and image resources for rendering
		createCommandPool();
		createCommandBuffers();
		createColorResources();
		createDepthResources();

		// creation of models and their representation for shaders
		createDescriptorSetLayout();

		sceneLoader.createDescriptorPoolsAndObjects();

		// creation of the graphics pipeline with render passes
		createRenderPass();
		createFramebuffers();
		createGraphicsPipeline();

		// create uniform buffers to pass to the shaders
		createSyncObjects();
	}

	void GraphicsEngine::createInstance()
	{
		// first of all, we check if the validation layer are requested and available
		if (useValidationLayers && !checkValidationLayerSupport())
		{
			throw std::runtime_error(getErrorStr(Error::VULKAN_FAIL_VALIDATION_LAYERS));
		}
		checkSupportedExtensions();

		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = title.c_str();
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		auto extensions = getRequiredExtensions();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
		if (useValidationLayers)
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size()); 
			createInfo.ppEnabledLayerNames = validationLayers.data();
			// debug messager is attached as extension to debug instance creation
			populateDebugMessengerCreateInfo(debugCreateInfo);
			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
		}
		else
		{
			createInfo.enabledLayerCount = 0;
			createInfo.pNext = nullptr;
		}

		if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
		{
			throw std::runtime_error(getErrorStr(Error::VULKAN_FAIL_CREATE_INSTANCE));
		}
	}

	vector<const char*> GraphicsEngine::getRequiredExtensions()
	{
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (useValidationLayers)
		{
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return extensions;
	}

	void GraphicsEngine::checkSupportedExtensions()
	{
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		vector<VkExtensionProperties> supportedExtensions(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, supportedExtensions.data());

		printAvailableExtensions(supportedExtensions);
		checkGLFWRequiredExtensions(supportedExtensions);
	}

	void GraphicsEngine::printAvailableExtensions(vector<VkExtensionProperties> extensions)
	{
		cout << "All the available Vulkan extensions are:" << endl;

		for (const auto& extension : extensions)
		{
			cout << '\t' << extension.extensionName << endl;
		}
	}

	void GraphicsEngine::checkGLFWRequiredExtensions(vector<VkExtensionProperties> extensions)
	{
		uint32_t requiredExtensionCount = 0;
		glfwGetRequiredInstanceExtensions(&requiredExtensionCount);
		const char** requiredExtensions = glfwGetRequiredInstanceExtensions(&requiredExtensionCount);

		cout << "Check if all the GLFW extensions are supported:" << endl;

		int unsupportedExtensions = 0;
		for (unsigned int i = 0; i < requiredExtensionCount; i++)
		{
			bool isSupported = false;
			for (const auto& extension : extensions)
			{
				if (strcmp(requiredExtensions[i], extension.extensionName) == 0)
				{
					isSupported = true;
					break;
				}
			}

			if (!isSupported)
			{
				unsupportedExtensions++;
			}
		}

		if (unsupportedExtensions == 0)
		{
			cout << "All GLFW required extensions are supported!" << endl;
		}
		else
		{
			cout << "There are " << unsupportedExtensions << " unsupported extensions of GLFW!" << endl;
		}
	}

	bool GraphicsEngine::checkValidationLayerSupport()
	{
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : validationLayers)
		{
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers)
			{
				if (strcmp(layerName, layerProperties.layerName) == 0)
				{
					layerFound = true;
					break;
				}
			}

			if (!layerFound)
			{
				return false;
			}
		}

		return true;
	}

	void GraphicsEngine::setupDebugMessenger()
	{
		if (useValidationLayers)
		{
			VkDebugUtilsMessengerCreateInfoEXT createInfo;
			populateDebugMessengerCreateInfo(createInfo);

			if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS)
			{
				throw std::runtime_error(getErrorStr(Error::VULKAN_FAIL_DEBUG_MESSENGER));
			}
		}
	}

	void GraphicsEngine::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo)
	{
		createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = debugCallback;
	}

	VkResult GraphicsEngine::CreateDebugUtilsMessengerEXT(VkInstance instance,
														  const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
														  const VkAllocationCallbacks* pAllocator,
														  VkDebugUtilsMessengerEXT* pDebugMessenger)
	{
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr)
		{
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		}
		else
		{
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	void GraphicsEngine::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr)
		{
			func(instance, debugMessenger, pAllocator);
		}
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL GraphicsEngine::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
																 VkDebugUtilsMessageTypeFlagsEXT messageType,
																 const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
																 void* pUserData)
	{
		if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
		{
			std::cerr << "Validation layer: " << pCallbackData->pMessage << endl;
		}

		return VK_FALSE;
	}

	void GraphicsEngine::pickPhysicalDevice()
	{
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

		if (deviceCount == 0)
		{
			throw std::runtime_error(getErrorStr(Error::VULKAN_FAIL_NO_GPU_PRESENT));
		}
		else
		{
			vector<VkPhysicalDevice> devices(deviceCount);
			vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
			std::multimap<int, VkPhysicalDevice> candidates;

			for (const auto &device : devices)
			{
				// if it is suitable, we rate it
				if (isDeviceSuitable(device))
				{
					int score = rateDeviceSuitability(device);
					candidates.insert(std::make_pair(score, device));
				}
			}

			// rbegin returns an iterator from the highest key to the lowest key
			// if there is at least one element, rbegin and rend point to different memory areas
			if (candidates.rbegin() != candidates.rend())
			{
				physicalDevice = candidates.rbegin()->second;
				msaaSamples = getMaximumMSAA();
			}

			if (physicalDevice == VK_NULL_HANDLE)
			{
				throw std::runtime_error(getErrorStr(Error::VULKAN_FAIL_NO_GPU_IS_SUITABLE));
			}
		}
	}

	void GraphicsEngine::createLogicalDevice() {
		QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

		vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

		// TODO: all queues have the same priority, is that what we want at the end?
		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies) {
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkPhysicalDeviceFeatures deviceFeatures{};
		deviceFeatures.samplerAnisotropy = VK_TRUE;
		deviceFeatures.sampleRateShading = VK_TRUE;

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());;
		createInfo.pQueueCreateInfos = queueCreateInfos.data();

		createInfo.pEnabledFeatures = &deviceFeatures;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();

		if (useValidationLayers) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		}
		else {
			createInfo.enabledLayerCount = 0;
		}
		if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
			throw std::runtime_error(getErrorStr(Error::VULKAN_FAIL_CREATE_LOGICAL_DEVICE));
		}
		vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
		vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
	}

	bool GraphicsEngine::isDeviceSuitable(VkPhysicalDevice device)
	{
		// TODO: is it not parametric in any way and it should be
		VkPhysicalDeviceFeatures deviceSupportedFeatures;
		vkGetPhysicalDeviceFeatures(device, &deviceSupportedFeatures);
		QueueFamilyIndices indices = findQueueFamilies(device);

		bool swapChainAdequate = false;
		bool extensionsSupported = checkDeviceExtensionSupport(device);
		if (extensionsSupported) {
			SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
			swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		}

		return deviceSupportedFeatures.geometryShader && indices.isComplete() && extensionsSupported && swapChainAdequate && deviceSupportedFeatures.samplerAnisotropy;
	}

	int GraphicsEngine::rateDeviceSuitability(VkPhysicalDevice device)
	{
		// TODO: it is not parametric in any way and it should be
		int score = 0;
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);

		// if the GPU is dedicated, it is much better
		if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			score += 1000;
		}

		return score;
	}

	QueueFamilyIndices GraphicsEngine::findQueueFamilies(VkPhysicalDevice device)
	{
		QueueFamilyIndices indices;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies)
		{
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				indices.graphicsFamily = i;
			}

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

			if (presentSupport)
			{
				indices.presentFamily = i;
			}

			if (indices.isComplete())
			{
				break;
			}

			i++;
		}

		return indices;
	}

	void GraphicsEngine::createSurface() {
		if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
			throw std::runtime_error(getErrorStr(Error::VULKAN_FAIL_CREATE_SURFACE));
		}
	}

	bool GraphicsEngine::checkDeviceExtensionSupport(VkPhysicalDevice device) {
		//TODO required extension may be passed as argument to this function instead of being declared here

		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		std::set<string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

		for (const auto& extension : availableExtensions)
		{
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	// TODO: the name is a bit misleading, it populates the swap chain structure, it does not check its capabilities
	SwapChainSupportDetails GraphicsEngine::querySwapChainSupport(VkPhysicalDevice device) {
		SwapChainSupportDetails details;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

		if (formatCount != 0)
		{
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

		if (presentModeCount != 0)
		{
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
		}

		return details;
	}

	VkSurfaceFormatKHR GraphicsEngine::chooseSwapSurfaceFormat(const vector<VkSurfaceFormatKHR> &availableFormats) {
		for (const auto &availableFormat : availableFormats)
		{
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				return availableFormat;
			}
		}

		// TODO: we should rank the available formats in case the standard SRGB is not available
		return availableFormats[0];
	}

	VkPresentModeKHR GraphicsEngine::chooseSwapPresentMode(const vector<VkPresentModeKHR> &availablePresentModes) {
		for (const auto &availablePresentMode : availablePresentModes)
		{
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				return availablePresentMode;
			}
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D GraphicsEngine::chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) {
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			return capabilities.currentExtent;
		}
		else
		{
			int width, height;
			glfwGetFramebufferSize(window, &width, &height);

			VkExtent2D actualExtent = {
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height)
			};

			actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
			actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

			return actualExtent;
		}
	}

	void GraphicsEngine::createSwapChain() {
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

		// populate the swap chain and set swap chain number of image to +1 without exceeding the maximum
		VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
		VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
		VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
		uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

		if (indices.graphicsFamily != indices.presentFamily) {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}

		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;

		if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
			throw std::runtime_error(getErrorStr(Error::VULKAN_FAIL_CREATE_SWAP_CHAIN));
		}

		vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
		swapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());
		swapChainImageFormat = surfaceFormat.format;
		swapChainExtent = extent;
	}

	void GraphicsEngine::createImageViews() {
		swapChainImageViews.resize(swapChainImages.size());
		for (uint32_t i = 0; i < swapChainImages.size(); i++)
		{
			swapChainImageViews[i] = createImageView(swapChainImages[i], swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
		}
	}

	VkShaderModule GraphicsEngine::createShaderModule(const vector<char>& code)
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
		VkShaderModule shaderModule;
		if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
		{
			throw std::runtime_error(getErrorStr(Error::VULKAN_FAIL_CREATE_SHADER_MODULE));
		}
		return shaderModule;
	}

	void GraphicsEngine::createGraphicsPipeline() {
		// TODO: we should generalize the creation of shaders and the attachment of shaders to object by API calls
		auto vertShaderCode = readFile("shaders/vert.spv");
		auto fragShaderCode = readFile("shaders/frag.spv");
		VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
		VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = vertShaderModule;
		vertShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = fragShaderModule;
		fragShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

		auto bindingDescription = Vertex::getBindingDescription();
		auto attributeDescriptions = Vertex::getAttributeDescriptions();

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;

		VkPipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;


		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_TRUE;
		multisampling.minSampleShading = 0.2f;
		multisampling.rasterizationSamples = msaaSamples;

		VkPipelineDepthStencilStateCreateInfo depthStencil{};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = VK_TRUE;
		depthStencil.depthWriteEnable = VK_TRUE;
		depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.stencilTestEnable = VK_FALSE;

		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_TRUE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

		VkPipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f; // Optional
		colorBlending.blendConstants[1] = 0.0f; // Optional
		colorBlending.blendConstants[2] = 0.0f; // Optional
		colorBlending.blendConstants[3] = 0.0f; // Optional

		std::vector<VkDynamicState> dynamicStates = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};
		VkPipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicState.pDynamicStates = dynamicStates.data();

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;

		if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error(getErrorStr(Error::VULKAN_FAIL_CREATE_PIPELINE_LAYOUT));
		}

		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = &depthStencil;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = &dynamicState;
		pipelineInfo.layout = pipelineLayout;
		pipelineInfo.renderPass = renderPass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

		if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
			throw std::runtime_error(getErrorStr(Error::VULKAN_FAIL_CREATE_GRAPHIC_PIPELINES));
		}

		vkDestroyShaderModule(device, fragShaderModule, nullptr);
		vkDestroyShaderModule(device, vertShaderModule, nullptr);
	}

	void GraphicsEngine::createRenderPass() {
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = swapChainImageFormat;
		colorAttachment.samples = msaaSamples;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentDescription depthAttachment{};
		depthAttachment.format = findDepthFormat();
		depthAttachment.samples = msaaSamples;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentDescription colorAttachmentResolve{};
		colorAttachmentResolve.format = swapChainImageFormat;
		colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentRef{};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference colorAttachmentResolveRef{};
		colorAttachmentResolveRef.attachment = 2;
		colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		subpass.pDepthStencilAttachment = &depthAttachmentRef;
		subpass.pResolveAttachments = &colorAttachmentResolveRef;

		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		std::array<VkAttachmentDescription, 3> attachments = { colorAttachment, depthAttachment, colorAttachmentResolve };
		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
			throw std::runtime_error(getErrorStr(Error::VULKAN_FAIL_CREATE_RENDER_PASS));
		}

	}

	void GraphicsEngine::createFramebuffers() {
		swapChainFramebuffers.resize(swapChainImageViews.size());
		for (size_t i = 0; i < swapChainImageViews.size(); i++) {
			std::array<VkImageView, 3> attachments = { colorImageView, depthImageView, swapChainImageViews[i] };

			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = renderPass;
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = swapChainExtent.width;
			framebufferInfo.height = swapChainExtent.height;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
				throw std::runtime_error(getErrorStr(Error::VULKAN_FAIL_CREATE_FRAME_BUFFER));
			}
		}
	}

	void GraphicsEngine::createCommandPool() {
		QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

		if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
		{
			throw std::runtime_error(getErrorStr(Error::VULKAN_FAIL_CREATE_COMMAND_POOL));
		}
	}

	void GraphicsEngine::createCommandBuffers() {
		commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

		if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS)
		{
			throw std::runtime_error(getErrorStr(Error::VULKAN_FAIL_ALLOCATE_COMMAND_BUFFER));
		}
	}
	
	void GraphicsEngine::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error(getErrorStr(Error::VULKAN_FAIL_BEGIN_RECORD_COMMAND_BUFFER));
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass;
		renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = swapChainExtent;

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
		clearValues[1].depthStencil = { 1.0f, 0 };

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)swapChainExtent.width;
		viewport.height = (float)swapChainExtent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = swapChainExtent;
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		for (int i = 0; i < sceneLoader.getNumOfObjects(); i++)
		{
			ObjectLoader* renderedObject = sceneLoader.getIthObject(i);

			VkBuffer vertexBuffers[] = { renderedObject->vertexBuffer };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
			vkCmdBindIndexBuffer(commandBuffer, renderedObject->indexBuffer, 0, VK_INDEX_TYPE_UINT32);
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &(renderedObject->descriptorSets[currentFrame]), 0, nullptr);
			vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(renderedObject->indices.size()), 1, 0, 0, 0);
		}

		vkCmdEndRenderPass(commandBuffer);
		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error(getErrorStr(Error::VULKAN_FAIL_RECORD_COMMAND_BUFFER));
		}
	}

	void GraphicsEngine::createSyncObjects() {
		imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
				vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
				vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS)
			{
				throw std::runtime_error(getErrorStr(Error::VULKAN_FAIL_CREATE_SYNCHRONIZATION_OBJECTS));
			}
		}
	}

	void GraphicsEngine::drawFrame() {
		vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

		uint32_t imageIndex;
		VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebuffersResized)
		{
			framebuffersResized = false;
			recreateSwapChain();
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw new std::runtime_error(getErrorStr(Error::VULKAN_FAIL_ACQUIRE_SWAP_CHAIN_IMAGE));
		}
		else
		{
			updateUniformBuffer(currentFrame);

			vkResetFences(device, 1, &inFlightFences[currentFrame]);

			vkResetCommandBuffer(commandBuffers[currentFrame], 0);
			recordCommandBuffer(commandBuffers[currentFrame], imageIndex);

			VkSubmitInfo submitInfo{};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

			VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
			VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = waitSemaphores;
			submitInfo.pWaitDstStageMask = waitStages;

			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

			VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = signalSemaphores;

			if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS)
			{
				throw std::runtime_error(getErrorStr(Error::VULKAN_FAIL_SUBMIT_DRAW_COMMAND_BUFFER));
			}

			VkPresentInfoKHR presentInfo{};
			presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

			presentInfo.waitSemaphoreCount = 1;
			presentInfo.pWaitSemaphores = signalSemaphores;

			VkSwapchainKHR swapChains[] = { swapChain };
			presentInfo.swapchainCount = 1;
			presentInfo.pSwapchains = swapChains;

			presentInfo.pImageIndices = &imageIndex;

			result = vkQueuePresentKHR(presentQueue, &presentInfo);

			if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebuffersResized)
			{
				framebuffersResized = false;
				recreateSwapChain();
			}
			else if (result != VK_SUCCESS)
			{
				throw new std::runtime_error(getErrorStr(Error::VULKAN_FAIL_ACQUIRE_SWAP_CHAIN_IMAGE));
			}

			currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
		}
	}

	void GraphicsEngine::recreateSwapChain()
	{
		int width = 0, height = 0;
		glfwGetFramebufferSize(window, &width, &height);
		while (width == 0 || height == 0)
		{
			glfwGetFramebufferSize(window, &width, &height);
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(device);

		cleanupSwapChain();

		createSwapChain();
		createImageViews();
		createColorResources();
		createDepthResources();
		createFramebuffers();
	}

	void GraphicsEngine::framebufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		auto app = reinterpret_cast<GraphicsEngine*>(glfwGetWindowUserPointer(window));
		app->framebuffersResized = true;
	}

	void GraphicsEngine::cleanupSwapChain()
	{
		vkDestroyImageView(device, colorImageView, nullptr);
		vkDestroyImage(device, colorImage, nullptr);
		vkFreeMemory(device, colorImageMemory, nullptr);

		vkDestroyImageView(device, depthImageView, nullptr);
		vkDestroyImage(device, depthImage, nullptr);
		vkFreeMemory(device, depthImageMemory, nullptr);

		for (auto framebuffer : swapChainFramebuffers)
		{
			vkDestroyFramebuffer(device, framebuffer, nullptr);
		}

		for (auto imageView : swapChainImageViews)
		{
			vkDestroyImageView(device, imageView, nullptr);
		}

		vkDestroySwapchainKHR(device, swapChain, nullptr);
	}

	// TODO: here uniform buffer objects are created as descriptor set to be passed to shaders
	void GraphicsEngine::createDescriptorSetLayout()
	{
		VkDescriptorSetLayoutBinding guboLayoutBinding{};
		guboLayoutBinding.binding = 0;
		guboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		guboLayoutBinding.descriptorCount = 1;
		guboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		guboLayoutBinding.pImmutableSamplers = nullptr;

		VkDescriptorSetLayoutBinding guboLightLayoutBinding{};
		guboLightLayoutBinding.binding = 1;
		guboLightLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		guboLightLayoutBinding.descriptorCount = 1;
		guboLightLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		guboLightLayoutBinding.pImmutableSamplers = nullptr;

		VkDescriptorSetLayoutBinding samplerLayoutBinding{};
		samplerLayoutBinding.binding = 2;
		samplerLayoutBinding.descriptorCount = 1;
		samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerLayoutBinding.pImmutableSamplers = nullptr;
		samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = 3;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.pImmutableSamplers = nullptr;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		VkDescriptorSetLayoutBinding uboLightLayoutBinding{};
		uboLightLayoutBinding.binding = 4;
		uboLightLayoutBinding.descriptorCount = 1;
		uboLightLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLightLayoutBinding.pImmutableSamplers = nullptr;
		uboLightLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		std::array<VkDescriptorSetLayoutBinding, 5> bindings = { guboLayoutBinding, guboLightLayoutBinding, samplerLayoutBinding, uboLayoutBinding, uboLightLayoutBinding };
		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();
		
		if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
		{
			throw std::runtime_error(getErrorStr(Error::VULKAN_FAIL_CREATE_DESCRIPTOR_SET_LAYOUT));
		}
	}

	void GraphicsEngine::updateUniformBuffer(uint32_t currentImage)
	{
		static auto startTime = std::chrono::high_resolution_clock::now();
		static auto lastFrameTime = std::chrono::high_resolution_clock::now();

		auto currentTime = std::chrono::high_resolution_clock::now();
		float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
		lastFrameTime = currentTime;

		physicsEngine->update(deltaTime);

		void* data;

		// load gubo of positions
		GlobalUniformBufferObject gubo{};
		gubo.view = activeScene->getCamera()->getCurrentTransform();
		gubo.proj = glm::perspective(glm::radians(45.0f), swapChainExtent.width / (float)swapChainExtent.height, nearPlane, farPlane);
		gubo.proj[1][1] *= -1;

		vkMapMemory(device, sceneLoader.globalUniformBuffersMemory[0][currentImage], 0, sizeof(gubo), 0, &data);
		memcpy(data, &gubo, sizeof(gubo));
		vkUnmapMemory(device, sceneLoader.globalUniformBuffersMemory[0][currentImage]);

		GlobalUniformBufferObjectLight guboLight{};
		// TODO: implement light

		vkMapMemory(device, sceneLoader.globalUniformBuffersMemory[1][currentImage], 0, sizeof(gubo), 0, &data);
		memcpy(data, &gubo, sizeof(gubo));
		vkUnmapMemory(device, sceneLoader.globalUniformBuffersMemory[1][currentImage]);

		UniformBufferObject ubo{};
		UniformBufferObjectLight uboLight{};
		for (int i = 0; i < sceneLoader.getNumOfObjects(); i++)
		{
			ObjectLoader* renderedObject = sceneLoader.getIthObject(i);
			GameObject* gameObject = activeScene->getAllGameObjects()[i];

			ubo.modelVertices = gameObject->getCurrentTransform();
			ubo.modelNormal = gameObject->getCurrentTransform();

			// TODO: implement object light

			vkMapMemory(device, renderedObject->objectUniformBuffersMemory[0][currentImage], 0, sizeof(ubo), 0, &data);
			memcpy(data, &ubo, sizeof(ubo));
			vkUnmapMemory(device, renderedObject->objectUniformBuffersMemory[0][currentImage]);

			vkMapMemory(device, renderedObject->objectUniformBuffersMemory[1][currentImage], 0, sizeof(uboLight), 0, &data);
			memcpy(data, &uboLight, sizeof(uboLight));
			vkUnmapMemory(device, renderedObject->objectUniformBuffersMemory[1][currentImage]);
		}
	}

	void GraphicsEngine::createDepthResources()
	{
		VkFormat depthFormat = findDepthFormat();

		createImage(swapChainExtent.width, swapChainExtent.height, 1, msaaSamples, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthImageMemory);

		depthImageView = createImageView(depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);

		// TODO: errors with the pipeline barrier, understand how to resolve
		// TODO: for some reasons the tutorial says that the following lines are optional, understand why
		//transitionImageLayout(depthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1);
	}

	VkFormat GraphicsEngine::findSupportedFormat(const vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
	{
		for (VkFormat format : candidates)
		{
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

			if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
			{
				return format;
			}
			else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
			{
				return format;
			}
		}

		throw std::runtime_error(getErrorStr(Error::IMAGE_FORMAT_REQUIREMENTS_TOO_STRICT));
	}

	VkFormat GraphicsEngine::findDepthFormat()
	{
		return findSupportedFormat(
			{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
		);
	}

	bool GraphicsEngine::hasStencilComponent(VkFormat format)
	{
		return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
	}

	VkSampleCountFlagBits GraphicsEngine::getMaximumMSAA()
	{
		VkPhysicalDeviceProperties physicalDeviceProperties;
		vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

		VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
		if (counts & VK_SAMPLE_COUNT_64_BIT)
		{
			return VK_SAMPLE_COUNT_64_BIT;
		}
		if (counts & VK_SAMPLE_COUNT_32_BIT)
		{
			return VK_SAMPLE_COUNT_32_BIT;
		}
		if (counts & VK_SAMPLE_COUNT_16_BIT)
		{
			return VK_SAMPLE_COUNT_16_BIT;
		}
		if (counts & VK_SAMPLE_COUNT_8_BIT)
		{
			return VK_SAMPLE_COUNT_8_BIT;
		}
		if (counts & VK_SAMPLE_COUNT_4_BIT)
		{
			return VK_SAMPLE_COUNT_4_BIT;
		}
		if (counts & VK_SAMPLE_COUNT_2_BIT)
		{
			return VK_SAMPLE_COUNT_2_BIT;
		}

		return VK_SAMPLE_COUNT_1_BIT;
	}

	void GraphicsEngine::createColorResources()
	{
		VkFormat colorFormat = swapChainImageFormat;

		// anti-aliasing if forced to have 1 mipmapping level since it is not used as a texture
		createImage(swapChainExtent.width, swapChainExtent.height, 1, msaaSamples, colorFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, colorImage, colorImageMemory);
		colorImageView = createImageView(colorImage, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
	}

	void GraphicsEngine::mainLoop()
	{
		while (!glfwWindowShouldClose(window))
		{
			glfwPollEvents();
			drawFrame();
		}

		vkDeviceWaitIdle(device);
	}

	void GraphicsEngine::cleanup()
	{
		cleanupSwapChain();

		vkDestroyPipeline(device, graphicsPipeline, nullptr);
		vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
		vkDestroyRenderPass(device, renderPass, nullptr);

		vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);

		sceneLoader.cleanup();

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
			vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
			vkDestroyFence(device, inFlightFences[i], nullptr);
		}

		vkDestroyCommandPool(device, commandPool, nullptr);

		vkDestroyDevice(device, nullptr);

		if (useValidationLayers)
		{
			DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
		}

		vkDestroySurfaceKHR(instance, surface, nullptr);
		vkDestroyInstance(instance, nullptr);

		glfwDestroyWindow(window);

		glfwTerminate();
	}

	/********************************************
	 *											*
	 *											*
	 *					HELPERS					*
	 *											*
	 *											*
	 *******************************************/

	VkCommandBuffer  GraphicsEngine::beginSingleTimeCommands()
	{
		return vulkanutils::beginSingleTimeCommands(device, commandPool);
	}

	void GraphicsEngine::endSingleTimeCommands(VkCommandBuffer commandBuffer)
	{
		vulkanutils::endSingleTimeCommands(device, commandPool, commandBuffer, graphicsQueue);
	}

	void GraphicsEngine::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
	{
		vulkanutils::createBuffer(physicalDevice, device, size, usage, properties, buffer, bufferMemory);
	}

	void GraphicsEngine::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
	{
		vulkanutils::copyBuffer(srcBuffer, dstBuffer, size, device, commandPool, graphicsQueue);
	}

	uint32_t GraphicsEngine::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
	{
		return vulkanutils::findMemoryType(physicalDevice, device, typeFilter, properties);
	}

	void GraphicsEngine::createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory)
	{
		vulkanutils::createImage(physicalDevice, device, width, height, mipLevels, numSamples, format, tiling, usage, properties, image, imageMemory);
	}

	VkImageView GraphicsEngine::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels)
	{
		return vulkanutils::createImageView(device, image, format, aspectFlags, mipLevels);
	}

	void GraphicsEngine::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels)
	{
		vulkanutils::transitionImageLayout(device, commandPool, graphicsQueue, image, format, oldLayout, newLayout, mipLevels);
	}

	void GraphicsEngine::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
	{
		vulkanutils::copyBufferToImage(device, commandPool, graphicsQueue, buffer, image, width, height);
	}

	void GraphicsEngine::generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels)
	{
		vulkanutils::generateMipmaps(physicalDevice, device, commandPool, graphicsQueue, image, imageFormat, texWidth, texHeight, mipLevels);
	}

	// TODO: this function could be extracted out from graphics engine and included in utils since reading is not Vulkan dependent
	vector<char> GraphicsEngine::readFile(const std::string& filename) {
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (!file.is_open())
		{
			throw std::runtime_error("failed to open file!");
		}

		size_t fileSize = (size_t)file.tellg();
		vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();

		return buffer;
	}
}