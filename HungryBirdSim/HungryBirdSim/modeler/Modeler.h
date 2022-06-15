//#pragma once
//
//
//#ifndef MODELER_H_
//#define MODELER_H_
//
//#define GLFW_INCLUDE_VULKAN
//#include <GLFW/glfw3.h>
//
//#include <string>
//#include <vector>
//using std::string;
//using std::vector;
//#define GLM_FORCE_RADIANS
//#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
//#define GLM_FORCE_DEPTH_ZERO_TO_ONE
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//
//#define TINYOBJLOADER_IMPLEMENTATION
//#include <tiny_obj_loader.h>
//
//#define STB_IMAGE_IMPLEMENTATION
//#include <stb_image.h>
//
//const string NO_PATH_INSERTED = "NOPATHINSERTEDXXSS";
//namespace Modeler
//{
//	const struct Vertex {
//		glm::vec3 pos;
//		glm::vec3 norm;
//		glm::vec2 texCoord;
//	};
//	const struct ModelDetails {
//		std::vector<Vertex> vertices;
//		std::vector<uint32_t> indices;
//	};
//	class Modeler
//	{
//	public:
//		Modeler(VkDevice device,string objPath = NO_PATH_INSERTED, string texturePath = NO_PATH_INSERTED,
//			std::vector<Vertex> vertices = std::vector<Vertex>(),
//			std::vector<uint32_t> indices = std::vector<uint32_t>());
//		ModelDetails createObject();
//		void setTexture();
//		void localInit();
//		void localCleanup();
//		void populateCommandBuffer();
//		void updateUniformBuffer();
//	private:
//		string objPath;
//		string texturePath;
//		std::vector<Vertex> vertices;
//		std::vector<uint32_t> indices;
//		VkDevice vkDevice;
//	};
//}
//
//#endif