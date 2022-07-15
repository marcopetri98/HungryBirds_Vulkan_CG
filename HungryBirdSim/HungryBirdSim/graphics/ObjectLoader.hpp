#pragma once

#ifndef OBJECTLOADER_HPP_
#define OBJECTLOADER_HPP_
#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#include <map>

#include "engine/GameObject.hpp"
#include "DummyRecursionSolver.hpp"
#include "SceneLoader.hpp"
#include "GraphicsEngine.h"
#include "Vertex.h"

using std::string;
using std::vector;
using std::map;

namespace graphics
{
	class ObjectLoader
	{
		public:
		ObjectLoader();
		ObjectLoader(GraphicsEngine* graphicsEngine, VkDescriptorPool* descriptorPool, SceneLoader* sceneLoader, int maximumFramesInFlight, GameObject gameObject);
		int getNumBuffers();
		int getNumSamplers();
		int getNumDescriptorSets();
		int getNumInstances();
		void incrementInstanceCount();
		void createModelAndBuffers();
		void createOnlyDescriptorSets();
		void cleanup();

		private:
		friend class GraphicsEngine;

		GameObject gameObject;
		GraphicsEngine* graphicsEngine;
		SceneLoader* sceneLoader;
		VkDescriptorPool* descriptorPool;
		int maximumFramesInFlight;
		int numInstances;

		// object specific values
		vector<Vertex> vertices;
		vector<uint32_t> indices;
		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		VkBuffer indexBuffer;
		VkDeviceMemory indexBufferMemory;
		uint32_t mipLevels;
		VkImage textureImage;
		VkDeviceMemory textureImageMemory;
		VkImageView textureImageView;
		VkSampler textureSampler;
		vector<vector<vector<VkBuffer>>> objectUniformBuffers;
		vector<vector<vector<VkDeviceMemory>>> objectUniformBuffersMemory;
		vector<vector<VkDescriptorSet>> descriptorSets;

		void loadModel();
		void createVertexBuffer();
		void createIndexBuffer();
		void createTextureImage();
		void createTextureImageView();
		void createTextureSampler();
		void createUniformBuffers(int instance);
		void createDescriptorSets(int instance);
	};
}

#endif