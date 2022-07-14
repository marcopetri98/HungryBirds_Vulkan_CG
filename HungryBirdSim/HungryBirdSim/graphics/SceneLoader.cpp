#include <stdexcept>

#include "../utils/VulkanUtils.hpp"
#include "../utils/Errors.h"
#include "DummyRecursionSolver.hpp"
#include "GraphicsEngine.h"
#include "SceneLoader.hpp"
#include "ObjectLoader.hpp"

using errors::getErrorStr;
using errors::Error;

namespace graphics
{
	SceneLoader::SceneLoader(GraphicsEngine* graphicsEngine, int maximumFramesInFlight, Scene scene)
	{
		this->graphicsEngine = graphicsEngine;
		this->maximumFramesInFlight = maximumFramesInFlight;
		this->scene = scene;
	}

	int SceneLoader::getNumOfObjects()
	{
		return this->objectLoaders.size();
	}

	ObjectLoader* SceneLoader::getIthObject(int i)
	{
		return &this->objectLoaders[i];
	}

	void SceneLoader::createDescriptorPoolsAndObjects()
	{
		createGlobalUniformBuffers();

		this->objectLoaders.clear();
		for (GameObject gameObject : this->scene.getAllGameObjects())
		{
			ObjectLoader objLoader = ObjectLoader(this->graphicsEngine, &(this->descriptorPool), this, this->maximumFramesInFlight, gameObject);
			objLoader.createModelAndBuffers();
			this->objectLoaders.push_back(objLoader);
		}

		createDescriptorPool();

		for (int i = 0; i < this->objectLoaders.size(); i++)
		{
			this->objectLoaders[i].createOnlyDescriptorSets();
		}
	}

	void SceneLoader::cleanup()
	{
		for (int i = 0; i < globalUniformBuffers.size(); i++)
		{
			for (size_t j = 0; j < globalUniformBuffers[0].size(); j++)
			{
				vkDestroyBuffer(graphicsEngine->device, globalUniformBuffers[i][j], nullptr);
				vkFreeMemory(graphicsEngine->device, globalUniformBuffersMemory[i][j], nullptr);
			}
		}

		vkDestroyDescriptorPool(graphicsEngine->device, descriptorPool, nullptr);

		for (int i = 0; i < this->objectLoaders.size(); i++)
		{
			this->objectLoaders[i].cleanup();
		}
	}

	void SceneLoader::createGlobalUniformBuffers()
	{
		vector<uint64_t> uboSizes = { sizeof(GlobalUniformBufferObject), sizeof(GlobalUniformBufferObjectLight) };

		globalUniformBuffers.resize(uboSizes.size());
		globalUniformBuffersMemory.resize(uboSizes.size());

		for (int i = 0; i < uboSizes.size(); i++)
		{
			VkDeviceSize bufferSize = uboSizes[i];

			globalUniformBuffers[i].resize(this->maximumFramesInFlight);
			globalUniformBuffersMemory[i].resize(this->maximumFramesInFlight);

			for (size_t j = 0; j < this->maximumFramesInFlight; j++)
			{
				vulkanutils::createBuffer(graphicsEngine->physicalDevice, graphicsEngine->device, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, globalUniformBuffers[i][j], globalUniformBuffersMemory[i][j]);
			}
		}
	}

	void SceneLoader::createDescriptorPool()
	{
		std::array<VkDescriptorPoolSize, 2> poolSizes{};
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[0].descriptorCount = static_cast<uint32_t>((1 + objectLoaders.size()) * objectLoaders[0].getNumBuffers());
		poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[1].descriptorCount = static_cast<uint32_t>((1 + objectLoaders.size()) * objectLoaders[0].getNumSamplers());

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = static_cast<uint32_t>(objectLoaders.size() * objectLoaders[0].getNumDescriptorSets());

		if (vkCreateDescriptorPool(graphicsEngine->device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
		{
			throw std::runtime_error(getErrorStr(Error::VULKAN_FAIL_CREATE_DESCRIPTOR_POOL));
		}
	}
}