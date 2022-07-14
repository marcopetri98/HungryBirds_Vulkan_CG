#pragma once

#ifndef SCENELOADER_HPP_
#define SCENELOADER_HPP_
#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#include <map>

#include "engine/Scene.hpp"
#include "DummyRecursionSolver.hpp"
#include "GraphicsEngine.h"

using std::string;
using std::vector;
using std::map;

namespace graphics
{
	class SceneLoader
	{
		public:
		SceneLoader(GraphicsEngine* graphicsEngine = NULL, int maximumFramesInFlight = 0, Scene scene = Scene());
		int getNumOfObjects();
		ObjectLoader* getIthObject(int i);
		void createDescriptorPoolsAndObjects();
		void cleanup();

		private:
		friend class GraphicsEngine;
		friend class ObjectLoader;

		Scene scene;
		GraphicsEngine* graphicsEngine;
		vector<ObjectLoader> objectLoaders;
		int maximumFramesInFlight;

		// scene specific fields
		VkDescriptorPool descriptorPool;
		vector<vector<VkBuffer>> globalUniformBuffers;
		vector<vector<VkDeviceMemory>> globalUniformBuffersMemory;

		void createDescriptorPool();
		void createGlobalUniformBuffers();
	};
}

#endif