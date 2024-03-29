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

			case Error::VULKAN_FAIL_VALIDATION_LAYERS:
				return "Validation layers have been requested. However, at least one of them is not available!";

			case Error::VULKAN_FAIL_DEBUG_MESSENGER:
				return "Debug messenger couldn't be set up, which means that the function has not been found!";

			case Error::VULKAN_FAIL_NO_GPU_PRESENT:
				return "There aren't GPUs that can be used by Vulkan!";

			case Error::VULKAN_FAIL_NO_GPU_IS_SUITABLE:
				return "Between the usable Vulkan GPUs, no one supports the requirements!";

			case Error::VULKAN_FAIL_CREATE_LOGICAL_DEVICE:
				return "Vulkan wasn't able to properly create the Logical device (VkDevice)!";

			case Error::VULKAN_FAIL_CREATE_SURFACE:
				return "Vulkan wasn't able to properly create the Vulkan surface (VkSurfaceKHR)!";

			case Error::VULKAN_FAIL_CREATE_SWAP_CHAIN:
				return "Vulkan wasn't able to properly create the Vulkan swap chain (VkSwapchainKHR)!";

			case Error::VULKAN_FAIL_CREATE_IMAGE_VIEWS:
				return "Vulkan wasn't able to properly create the Vulkan image views (VkImageView)!";

			case Error::VULKAN_FAIL_CREATE_SHADER_MODULE:
				return "Vulkan wasn't able to properly create the Vulkan shader module (VkShaderModule)!";

			case Error::VULKAN_FAIL_CREATE_PIPELINE_LAYOUT:
				return "Vulkan wasn't able to properly create the Vulkan pipeline layout (VkPipelineLayout)!";

			case Error::VULKAN_FAIL_CREATE_RENDER_PASS:
				return "Vulkan wasn't able to properly create the Vulkan render pass (VkRenderPass)!";

			case Error::VULKAN_FAIL_CREATE_GRAPHIC_PIPELINES:
				return "Vulkan wasn't able to properly create the Vulkan graphics pipeline (VkPipeline)!";

			case Error::VULKAN_FAIL_CREATE_FRAME_BUFFER:
				return "Vulkan wasn't able to properly create the Vulkan frame buffers (VkFramebuffer)!";

			case Error::VULKAN_FAIL_CREATE_COMMAND_POOL:
				return "Vulkan wasn't able to properly create the Vulkan command pool (VkCommandPool)!";

			case Error::VULKAN_FAIL_ALLOCATE_COMMAND_BUFFER:
				return "Vulkan wasn't able to properly allocate the Vulkan command buffers (VkCommandBuffer)!";

			case Error::VULKAN_FAIL_BEGIN_RECORD_COMMAND_BUFFER:
				return "Vulkan wasn't able to properly begin recording a command buffer (VkCommandBufferBeginInfo and VkCommandBuffer)!";

			case Error::VULKAN_FAIL_RECORD_COMMAND_BUFFER:
				return "Vulkan wasn't able to properly end recording a command buffer (VkCommandBuffer)!";

			case Error::VULKAN_FAIL_SUBMIT_DRAW_COMMAND_BUFFER:
				return "Vulkan wasn't able to properly submit a draw command buffer (drawFrame function)!";

			case Error::VULKAN_FAIL_CREATE_SYNCHRONIZATION_OBJECTS:
				return "Vulkan wasn't able to properly create synchronization objects (VkSemaphore or VkFence)!";

			case Error::VULKAN_FAIL_ACQUIRE_SWAP_CHAIN_IMAGE:
				return "Vulkan wasn't able to properly acquire the swap chain images (vkAcquireNextImageKHR)!";

			case Error::VULKAN_FAIL_CREATE_VERTEX_BUFFER:
				return "Vulkan wasn't able to properly create the vertex buffer (VkBuffer)!";

			case Error::VULKAN_FAIL_FIND_SUITABLE_MEMORY_TYPE:
				return "Vulkan wasn't able to properly find a suitable memory type for the vertex buffer!";

			case Error::VULKAN_FAIL_ALLOCATE_VERTEX_BUFFER_MEMORY:
				return "Vulkan wasn't able to properly allocate the found memory for the vertex (VkDeviceMemory)!";

			case Error::VULKAN_FAIL_CREATE_DESCRIPTOR_SET_LAYOUT:
				return "Vulkan wasn't able to properly allocate the descriptor set layout (VkDescriptorSetLayout)!";

			case Error::VULKAN_FAIL_CREATE_DESCRIPTOR_POOL:
				return "Vulkan wasn't able to properly allocate the descriptor pool (VkDescriptorPool)!";

			case Error::VULKAN_FAIL_ALLOCATE_DESCRIPTOR_SET:
				return "Vulkan wasn't able to properly allocate the descriptor set (VkDescriptorSet)!";

			case Error::STB_FAIL_LOAD_IMAGE:
				return "The loading of a texture image using STB wasn't successful! Control paths!";

			case Error::VULKAN_FAIL_CREATE_IMAGE:
				return "Vulkan wasn't able to properly create an image (VkImage)!";

			case Error::VULKAN_FAIL_ALLOCATE_IMAGE:
				return "Vulkan wasn't able to allocate an image with the specified requirements!";

			case Error::VULKAN_FAIL_DOESNT_SUPPORT_TRANSITION_LAYOUT:
				return "Vulkan does not support the specified transition layout (VkImageMemoryBarrier)!";

			case Error::VULKAN_FAIL_CREATE_TEXTURE_SAMPLER:
				return "Vulkan wasn't able to properly create the texture sampler (VkSampler)!";

			case Error::IMAGE_FORMAT_REQUIREMENTS_TOO_STRICT:
				return "The requirements for the image were too strict and no format can be used!";

			case Error::VULKAN_FAIL_IMAGE_BLITTING:
				return "Vulkan wasn't able to properly perform blitting on the image since it is not supported!";
		}
	}

	string getVulkanErrorStr(VkResult result)
	{
		switch (result)
		{
			case VK_ERROR_OUT_OF_HOST_MEMORY:
				return "[ERROR OUT OF HOST MEMORY]: ";

			case VK_ERROR_OUT_OF_DEVICE_MEMORY:
				return "[ERROR OUT OF DEVICE MEMORY]: ";

			case VK_ERROR_FRAGMENTED_POOL:
				return "[ERROR FRAGMENTED POOL]: ";

			case VK_ERROR_OUT_OF_POOL_MEMORY:
				return "[ERROR OUT OF POOL MEMORY]: ";

			default:
				return "[VULKAN ERROR NOT MAPPED IN FUNCTION] ";
		}
	}
}
