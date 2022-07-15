#include <stdexcept>

#include "../utils/CollectionUtils.hpp"
#include "../utils/VulkanUtils.hpp"
#include "../utils/Errors.h"
#include "DummyRecursionSolver.hpp"
#include "GraphicsEngine.h"
#include "SceneLoader.hpp"
#include "ObjectLoader.hpp"

using errors::getErrorStr;
using errors::Error;
using std::pair;
using std::string;
using std::vector;
using collectionutils::vectorContains;

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

	int SceneLoader::getTotalNumBuffers()
	{
		int totalNumBuffers = 0;

		for (int i = 0; i < objectLoaders.size(); i++)
		{
			totalNumBuffers += objectLoaders[i].getNumBuffers();
		}

		return totalNumBuffers;
	}

	int SceneLoader::getTotalNumDescriptorSets()
	{
		int totalNumDescriptorSets = 0;

		for (int i = 0; i < objectLoaders.size(); i++)
		{
			totalNumDescriptorSets += objectLoaders[i].getNumDescriptorSets();
		}

		return totalNumDescriptorSets;
	}

	int SceneLoader::getTotalNumSamplers()
	{
		int totalNumSamplers = 0;

		for (int i = 0; i < objectLoaders.size(); i++)
		{
			totalNumSamplers += objectLoaders[i].getNumSamplers();
		}

		return totalNumSamplers;
	}

	void SceneLoader::createDescriptorPoolsAndObjects()
	{
		createGlobalUniformBuffers();

		this->objectLoaders.clear();
		this->objectsTypes.clear();
		this->mapTypeToLoaderPos.clear();
		for (GameObject* gameObject : this->scene.getAllGameObjects())
		{
			pair<string, string> objectDescription = std::make_pair(gameObject->getObjectPath(), gameObject->getTexturePath());
			
			if (vectorContains(this->objectsTypes, objectDescription))
			{
				int loaderPos = this->mapTypeToLoaderPos[objectDescription];
				objectLoaders[loaderPos].incrementInstanceCount();
			}
			else
			{
				ObjectLoader objLoader = ObjectLoader(this->graphicsEngine, &(this->descriptorPool), this, this->maximumFramesInFlight, *gameObject);
				this->objectLoaders.push_back(objLoader);
				this->objectsTypes.push_back(objectDescription);
				this->mapTypeToLoaderPos.insert(pair<pair<string, string>, int>(objectDescription, this->objectsTypes.size() - 1));
			}
		}
		for (int i = 0; i < objectLoaders.size(); i++)
		{
			objectLoaders[i].createModelAndBuffers();
		}

		if (this->scene.getBackgroundPointer() != NULL)
		{
			GameObject backgroundGameObject = GameObject("background", {}, this->scene.getBackground().getObjectPath(), this->scene.getBackground().getTexturePath());
			backgroundLoader = new ObjectLoader(this->graphicsEngine, &(this->descriptorPool), this, this->maximumFramesInFlight, backgroundGameObject);
			backgroundLoader->createModelAndBuffers();
		}

		createDescriptorPool();

		for (int i = 0; i < this->objectLoaders.size(); i++)
		{
			this->objectLoaders[i].createOnlyDescriptorSets();
		}
		if (this->scene.getBackgroundPointer() != NULL)
		{
			backgroundLoader->createOnlyDescriptorSets();
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

		if (this->scene.getBackgroundPointer() != NULL)
		{
			backgroundLoader->cleanup();
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
		int numUniform = ((this->scene.getBackgroundPointer() == NULL ? 4 : 4 + backgroundLoader->getNumBuffers()) + getTotalNumBuffers()) * 3;
		int numSamplers = (this->scene.getBackgroundPointer() == NULL ? 0 : backgroundLoader->getNumSamplers()) + getTotalNumSamplers();
		int numDescriptors = (this->scene.getBackgroundPointer() == NULL ? 0 : backgroundLoader->getNumDescriptorSets()) + getTotalNumDescriptorSets();

		// TODO: see why vulkan on some pcs is happy with 0 descriptorCount and in other pcs is happy with only at least double the required number
		std::array<VkDescriptorPoolSize, 2> poolSizes{};
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[0].descriptorCount = static_cast<uint32_t>(numUniform);
		poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[1].descriptorCount = static_cast<uint32_t>(numSamplers);

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = static_cast<uint32_t>(numDescriptors);

		if (vkCreateDescriptorPool(graphicsEngine->device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
		{
			throw std::runtime_error(getErrorStr(Error::VULKAN_FAIL_CREATE_DESCRIPTOR_POOL));
		}
	}
}