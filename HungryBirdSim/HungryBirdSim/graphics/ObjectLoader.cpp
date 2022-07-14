#include <stb_image.h>
#include <tiny_obj_loader.h>

#include <stdexcept>

#include "../utils/VulkanUtils.hpp"
#include "../utils/Errors.h"
#include "ObjectLoader.hpp"

using errors::getErrorStr;
using errors::getVulkanErrorStr;
using errors::Error;

namespace graphics
{
	ObjectLoader::ObjectLoader(GraphicsEngine* graphicsEngine, VkDescriptorPool* descriptorPool, SceneLoader* sceneLoader, int maximumFramesInFlight, GameObject gameObject)
	{
		this->gameObject = gameObject;
		this->graphicsEngine = graphicsEngine;
		this->sceneLoader = sceneLoader;
		this->descriptorPool = descriptorPool;
		this->maximumFramesInFlight = maximumFramesInFlight;
	}

	void ObjectLoader::createModelAndBuffers()
	{
		loadModel();
		createVertexBuffer();
		createIndexBuffer();
		createTextureImage();
		createTextureImageView();
		createTextureSampler();
		createUniformBuffers();
	}

	void ObjectLoader::createOnlyDescriptorSets()
	{
		createDescriptorSets();
	}

	int ObjectLoader::getNumBuffers()
	{
		if (objectUniformBuffers.size() == 0)
		{
			return 0;
		}
		else
		{
			return objectUniformBuffers.size() * objectUniformBuffers[0].size();
		}
	}

	int ObjectLoader::getNumSamplers()
	{
		return 1;
	}

	int ObjectLoader::getNumDescriptorSets()
	{
		return this->maximumFramesInFlight;
	}

	void ObjectLoader::cleanup()
	{
		vkDestroySampler(graphicsEngine->device, textureSampler, nullptr);
		vkDestroyImageView(graphicsEngine->device, textureImageView, nullptr);

		vkDestroyImage(graphicsEngine->device, textureImage, nullptr);
		vkFreeMemory(graphicsEngine->device, textureImageMemory, nullptr);

		vkDestroyBuffer(graphicsEngine->device, indexBuffer, nullptr);
		vkFreeMemory(graphicsEngine->device, indexBufferMemory, nullptr);

		vkDestroyBuffer(graphicsEngine->device, vertexBuffer, nullptr);
		vkFreeMemory(graphicsEngine->device, vertexBufferMemory, nullptr);

		for (int i = 0; i < objectUniformBuffers.size(); i++)
		{
			for (size_t j = 0; j < objectUniformBuffers[0].size(); j++)
			{
				vkDestroyBuffer(graphicsEngine->device, objectUniformBuffers[i][j], nullptr);
				vkFreeMemory(graphicsEngine->device, objectUniformBuffersMemory[i][j], nullptr);
			}
		}
	}

	void ObjectLoader::loadModel()
	{
		tinyobj::attrib_t attrib;
		vector<tinyobj::shape_t> shapes;
		vector<tinyobj::material_t> materials;
		string warn, err;
		
		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, gameObject.getObjectPath().c_str()))
		{
			throw std::runtime_error("ObjectLoader: " + warn + err);
		}

		std::unordered_map<Vertex, uint32_t> uniqueVertices{};
		for (const auto& shape : shapes)
		{
			for (const auto& index : shape.mesh.indices)
			{
				Vertex vertex{};

				vertex.pos = {
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2]
				};

				vertex.texCoord = {
					attrib.texcoords[2 * index.texcoord_index + 0],
					1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
				};

				vertex.color = { 1.0f, 1.0f, 1.0f };

				if (uniqueVertices.count(vertex) == 0)
				{
					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vertex);
				}

				indices.push_back(uniqueVertices[vertex]);
			}
		}
	}

	void ObjectLoader::createVertexBuffer()
	{
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		vulkanutils::createBuffer(graphicsEngine->physicalDevice, graphicsEngine->device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(graphicsEngine->device, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), (size_t)bufferSize);
		vkUnmapMemory(graphicsEngine->device, stagingBufferMemory);

		vulkanutils::createBuffer(graphicsEngine->physicalDevice, graphicsEngine->device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

		vulkanutils::copyBuffer(stagingBuffer, vertexBuffer, bufferSize, graphicsEngine->device, graphicsEngine->commandPool, graphicsEngine->graphicsQueue);

		vkDestroyBuffer(graphicsEngine->device, stagingBuffer, nullptr);
		vkFreeMemory(graphicsEngine->device, stagingBufferMemory, nullptr);
	}

	void ObjectLoader::createIndexBuffer()
	{
		VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		vulkanutils::createBuffer(graphicsEngine->physicalDevice, graphicsEngine->device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(graphicsEngine->device, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, indices.data(), (size_t)bufferSize);
		vkUnmapMemory(graphicsEngine->device, stagingBufferMemory);

		vulkanutils::createBuffer(graphicsEngine->physicalDevice, graphicsEngine->device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

		vulkanutils::copyBuffer(stagingBuffer, indexBuffer, bufferSize, graphicsEngine->device, graphicsEngine->commandPool, graphicsEngine->graphicsQueue);

		vkDestroyBuffer(graphicsEngine->device, stagingBuffer, nullptr);
		vkFreeMemory(graphicsEngine->device, stagingBufferMemory, nullptr);
	}

	void ObjectLoader::createTextureImage()
	{
		int texWidth, texHeight, texChannels;
		stbi_uc* pixels = stbi_load(gameObject.getTexturePath().c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		VkDeviceSize imageSize = texWidth * texHeight * 4;
		mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

		if (!pixels)
		{
			throw std::runtime_error(getErrorStr(Error::STB_FAIL_LOAD_IMAGE));
		}

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		vulkanutils::createBuffer(graphicsEngine->physicalDevice, graphicsEngine->device, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(graphicsEngine->device, stagingBufferMemory, 0, imageSize, 0, &data);
		memcpy(data, pixels, static_cast<size_t>(imageSize));
		vkUnmapMemory(graphicsEngine->device, stagingBufferMemory);

		stbi_image_free(pixels);

		vulkanutils::createImage(graphicsEngine->physicalDevice, graphicsEngine->device, texWidth, texHeight, mipLevels, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory);
		vulkanutils::transitionImageLayout(graphicsEngine->device, graphicsEngine->commandPool, graphicsEngine->graphicsQueue, textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels);
		vulkanutils::copyBufferToImage(graphicsEngine->device, graphicsEngine->commandPool, graphicsEngine->graphicsQueue, stagingBuffer, textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));

		vkDestroyBuffer(graphicsEngine->device, stagingBuffer, nullptr);
		vkFreeMemory(graphicsEngine->device, stagingBufferMemory, nullptr);

		vulkanutils::generateMipmaps(graphicsEngine->physicalDevice, graphicsEngine->device, graphicsEngine->commandPool, graphicsEngine->graphicsQueue, textureImage, VK_FORMAT_R8G8B8A8_SRGB, texWidth, texHeight, mipLevels);
	}

	void ObjectLoader::createTextureImageView()
	{
		textureImageView = vulkanutils::createImageView(graphicsEngine->device, textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels);
	}

	void ObjectLoader::createTextureSampler()
	{
		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(graphicsEngine->physicalDevice, &properties);

		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = VK_TRUE;

		// TODO: modify here to let use choose from API Anistropic filter dimension (pff pff, damn gooood, we SHOULD DO THAT MEN)
		samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;

		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.minLod = 0.0f; // Optional
		samplerInfo.maxLod = static_cast<float>(mipLevels);
		samplerInfo.mipLodBias = 0.0f; // Optional

		if (vkCreateSampler(graphicsEngine->device, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS)
		{
			throw std::runtime_error(getErrorStr(Error::VULKAN_FAIL_CREATE_TEXTURE_SAMPLER));
		}
	}

	void ObjectLoader::createUniformBuffers()
	{
		vector<uint64_t> uboSizes = { sizeof(UniformBufferObject), sizeof(UniformBufferObjectLight) };

		objectUniformBuffers.resize(uboSizes.size());
		objectUniformBuffersMemory.resize(uboSizes.size());

		for (int i = 0; i < uboSizes.size(); i++)
		{
			VkDeviceSize bufferSize = uboSizes[i];

			objectUniformBuffers[i].resize(this->maximumFramesInFlight);
			objectUniformBuffersMemory[i].resize(this->maximumFramesInFlight);

			for (size_t j = 0; j < this->maximumFramesInFlight; j++)
			{
				vulkanutils::createBuffer(graphicsEngine->physicalDevice, graphicsEngine->device, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, objectUniformBuffers[i][j], objectUniformBuffersMemory[i][j]);
			}
		}
	}

	void ObjectLoader::createDescriptorSets()
	{
		vector<VkDescriptorSetLayout> layouts(this->maximumFramesInFlight, graphicsEngine->descriptorSetLayout);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = *descriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(this->maximumFramesInFlight);
		allocInfo.pSetLayouts = layouts.data();

		descriptorSets.resize(this->maximumFramesInFlight);
		auto result = vkAllocateDescriptorSets(graphicsEngine->device, &allocInfo, descriptorSets.data());
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error(getVulkanErrorStr(result) + getErrorStr(Error::VULKAN_FAIL_ALLOCATE_DESCRIPTOR_SET));
		}

		for (size_t i = 0; i < this->maximumFramesInFlight; i++)
		{
			// TODO: we could create a struct of some field information to automatically generate these fields
			VkDescriptorBufferInfo guboBufferInfo{};
			guboBufferInfo.buffer = sceneLoader->globalUniformBuffers[0][i];
			guboBufferInfo.offset = 0;
			guboBufferInfo.range = sizeof(GlobalUniformBufferObject);

			VkDescriptorBufferInfo guboLightBufferInfo{};
			guboLightBufferInfo.buffer = sceneLoader->globalUniformBuffers[1][i];
			guboLightBufferInfo.offset = 0;
			guboLightBufferInfo.range = sizeof(GlobalUniformBufferObjectLight);

			VkDescriptorImageInfo imageInfo{};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = textureImageView;
			imageInfo.sampler = textureSampler;

			VkDescriptorBufferInfo uboBufferInfo{};
			uboBufferInfo.buffer = objectUniformBuffers[0][i];
			uboBufferInfo.offset = 0;
			uboBufferInfo.range = sizeof(UniformBufferObject);

			VkDescriptorBufferInfo uboLightBufferInfo{};
			uboLightBufferInfo.buffer = objectUniformBuffers[1][i];
			uboLightBufferInfo.offset = 0;
			uboLightBufferInfo.range = sizeof(UniformBufferObjectLight);

			std::array<VkWriteDescriptorSet, 5> descriptorWrites{};

			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = descriptorSets[i];
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &guboBufferInfo;

			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet = descriptorSets[i];
			descriptorWrites[1].dstBinding = 1;
			descriptorWrites[1].dstArrayElement = 0;
			descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[1].descriptorCount = 1;
			descriptorWrites[1].pBufferInfo = &guboLightBufferInfo;

			descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[2].dstSet = descriptorSets[i];
			descriptorWrites[2].dstBinding = 3;
			descriptorWrites[2].dstArrayElement = 0;
			descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[2].descriptorCount = 1;
			descriptorWrites[2].pBufferInfo = &uboBufferInfo;

			descriptorWrites[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[3].dstSet = descriptorSets[i];
			descriptorWrites[3].dstBinding = 4;
			descriptorWrites[3].dstArrayElement = 0;
			descriptorWrites[3].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[3].descriptorCount = 1;
			descriptorWrites[3].pBufferInfo = &uboLightBufferInfo;

			descriptorWrites[4].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[4].dstSet = descriptorSets[i];
			descriptorWrites[4].dstBinding = 2;
			descriptorWrites[4].dstArrayElement = 0;
			descriptorWrites[4].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[4].descriptorCount = 1;
			descriptorWrites[4].pImageInfo = &imageInfo;

			vkUpdateDescriptorSets(graphicsEngine->device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}
	}
}
