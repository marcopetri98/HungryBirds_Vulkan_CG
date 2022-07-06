#pragma once

#ifndef GRAPHICSENGINE_H_
#define GRAPHICSENGINE_H_
#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <string>
#include <vector>

#include "Queues.h"
#include "SwapChainSupportDetails.h"
#include "Vertex.h"
#include "UniformObjects.h"

using std::string;
using std::vector;

namespace graphics
{
	const string DEFAULT_TITLE = "Vulkan application";
	const int DEFAULT_WIDTH = 800;
	const int DEFAULT_HEIGHT = 600;
	const int MAX_FRAMES_IN_FLIGHT = 2;
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
		VkRenderPass renderPass;
		VkDescriptorSetLayout descriptorSetLayout;
		VkPipelineLayout pipelineLayout;
		VkPipeline graphicsPipeline;
		VkCommandPool commandPool;
		uint32_t currentFrame = 0;
		vector<VkCommandBuffer> commandBuffers;
		vector<VkSemaphore> imageAvailableSemaphores;
		vector<VkSemaphore> renderFinishedSemaphores;
		vector<VkFence> inFlightFences;
		bool framebuffersResized = false;
		// TODO: the triangle is stored in this buffer (I assume that multiple objects will mean multiple vertex buffer)
		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		VkBuffer indexBuffer;
		VkDeviceMemory indexBufferMemory;
		vector<VkBuffer> uniformBuffers;
		vector<VkDeviceMemory> uniformBuffersMemory;
		VkDescriptorPool descriptorPool;
		vector<VkDescriptorSet> descriptorSets;

		vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};
		vector<VkImage> swapChainImages;
		vector<VkImageView> swapChainImageViews;
		vector<VkFramebuffer> swapChainFramebuffers;

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
		VkSurfaceFormatKHR chooseSwapSurfaceFormat(const vector<VkSurfaceFormatKHR>& availableFormats);
		/**
		 * Chooses the presentation format. If MailboxKHR is supported is chosen, else FifoKHR is used.
		 *
		 * @param availableFormats the available formats
		 * @return the chosen surface format
		 */
		VkPresentModeKHR chooseSwapPresentMode(const vector<VkPresentModeKHR>& availablePresentModes);
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
		/**
		 * Create the graphics pipeline
		 */
		void createGraphicsPipeline();
		/**
		 * Helper function to load the binary data from a file.
		 * 
		 * @param filename Name of the file to read from
		 * @return The read binary data
		 */
		static std::vector<char> readFile(const string& filename);
		/**
		 * Helper function to wrap the shader code in a VkShaderModule object
		 * 
		 * @param code the shader code to be wrapped
		 * @return the VKShaderModule wrapping the given shader
		 */
		VkShaderModule createShaderModule(const vector<char>& code);
		/**
		 * Creates render pass object
		 */
		void createRenderPass();
		/**
		 * Create the required frame buffers
		 */
		void createFramebuffers();
		/**
		 * Create the command pool
		 * 
		 */
		void createCommandPool();
		/**
		 * Create the command buffer
		 * 
		 */
		void createCommandBuffers();
		/**
		 * Writes the commands into a command buffer for execution
		 * 
		 * @param commandBuffer
		 * @param imageIndex
		 */
		void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
		/**
		 * Draws the triangle (current test) on screen
		 * 
		 */
		void drawFrame();
		/**
		 * Creates two semaphores and a fence to handle synchronization
		 * 
		 */
		void createSyncObjects();
		/**
		 * Recreate the swap change due to an event that made it invalid, such as window resizing.
		 * 
		 */
		void recreateSwapChain();
		/**
		 * Clean up all the swap chain related resources and class members.
		 * 
		 */
		void cleanupSwapChain();
		/**
		 * Creates the vertex buffer to be able to send to shaders vertices from Vulkan.
		 * 
		 */
		void createVertexBuffer();
		/**
		 * Find the memory best suiting the application requirements.
		 * 
		 * @param typeFilter Memories supporting the vertex buffer requrements.
		 * @param properties The flags identifying the properties we desire the memory to have.
		 * @return index of the first memory satisfying all requirements.
		 */
		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
		/**
		 * Creates a buffer with as it is specified in the parameters.
		 * 
		 * @param size The dimension of the buffer.
		 * @param usage Flags stating how the buffer will be used.
		 * @param properties Properties of the buffer.
		 * @param buffer The buffer to be filled.
		 * @param bufferMemory The memory to be used for that buffer.
		 */
		void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
		/**
		 * Copy the contents of a source buffer to a destination buffer.
		 * 
		 * @param srcBuffer The source buffer.
		 * @param dstBuffer The destination buffer.
		 * @param size The size of the buffers.
		 */
		void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		/**
		 * Creates the index buffer to optimize memory used by vertices.
		 * 
		 */
		void createIndexBuffer();
		/**
		 * Create descriptor set layout to be able to pass uniform objects to shaders.
		 * 
		 */
		void createDescriptorSetLayout();
		/**
		 * Creates the uniform buffers to be filled and used with description set layouts.
		 * 
		 */
		void createUniformBuffers();
		/**
		 * Updates the current uniform buffer object for rendering..
		 * 
		 * @param currentImage The current image on which we need to draw.
		 */
		void updateUniformBuffer(uint32_t currentImage);
		/**
		 * Creates the descriptor pool to create all the needed descriptor sets.
		 * 
		 */
		void createDescriptorPool();
		/**
		 * Create the descriptor sets.
		 * 
		 */
		void createDescriptorSets();
		void mainLoop();
		void cleanup();

		static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
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