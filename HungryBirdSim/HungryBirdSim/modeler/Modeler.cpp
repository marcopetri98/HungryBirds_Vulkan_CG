//#include "Modeler.h"
//#include <string>
//#include <vector>
//using std::string;
//using std::vector;
//// GLM Headers
//#define GLM_FORCE_RADIANS
//#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
//#define GLM_FORCE_DEPTH_ZERO_TO_ONE
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//
//// TINYOBJ Headers
//#define TINYOBJLOADER_IMPLEMENTATION
//#include <tiny_obj_loader.h>
//namespace Modeler {
//
//	/**  The vector of verticesV you pass is the vector of vertices that in the end
//	** will be sent to the shaders. Same for the indicesV.
//	** */
//	Modeler::Modeler(VkDevice vkdevice, string objPathV,string texturePathV , std::vector<Vertex> verticesV , std::vector<uint32_t> indicesV)
//	{
//		this->vkDevice = vkdevice;
//		this->objPath = objPathV;
//		this->texturePath = texturePathV;
//		this->vertices = verticesV;
//		this->indices = indicesV;
//	}
//	/* Model Details is a class which contains a vector of vertices in a var called .vertices
//	* and a vector of <uint32_t> indices.
//	*/
//	ModelDetails Modeler::createObject() {
//		ModelDetails modelDetails{  };
//		modelDetails.vertices;
//		modelDetails.indices;
//		tinyobj::attrib_t attrib;
//		std::vector<tinyobj::shape_t> shapes;
//		std::vector<tinyobj::material_t> materials;
//		std::string warn, err;
//
//		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, objPath.c_str())) {
//			throw std::runtime_error(warn + err);
//		}
//
//		for (const auto& shape : shapes) {
//			for (const auto& index : shape.mesh.indices) {
//				Vertex vertex{};
//
//				vertex.pos = {
//					attrib.vertices[3 * index.vertex_index + 0],
//					attrib.vertices[3 * index.vertex_index + 1],
//					attrib.vertices[3 * index.vertex_index + 2]
//				};
//
//				vertex.texCoord = {
//					attrib.texcoords[2 * index.texcoord_index + 0],
//					1 - attrib.texcoords[2 * index.texcoord_index + 1]
//				};
//
//				vertex.norm = {
//					attrib.normals[3 * index.normal_index + 0],
//					attrib.normals[3 * index.normal_index + 1],
//					attrib.normals[3 * index.normal_index + 2]
//				};
//
//				this->vertices.push_back(vertex);
//				this->indices.push_back(vertices.size() - 1);
//			}
//		}
//		modelDetails.vertices = this->vertices;
//		modelDetails.indices = this->indices;
//		return modelDetails;
//	}
//
//	void Modeler::setTexture() {
//		
//		/*
//		int texWidth, texHeight, texChannels;
//		stbi_uc* pixels = stbi_load(texturePath.c_str(), &texWidth, &texHeight,
//			&texChannels, STBI_rgb_alpha);
//		if (!pixels) {
//			throw std::runtime_error("failed to load texture image!");
//		}
//
//		VkDeviceSize imageSize = texWidth * texHeight * 4;
//		// creates MIPS
//		// Maybe skip?
//		// mipLevels = static_cast<uint32_t>(std::floor(
//		//	std::log2(std::max(texWidth, texHeight)))) + 1;
//		int mipLevels = 1;
//
//		VkBuffer stagingBuffer;
//		VkDeviceMemory stagingBufferMemory;
//
//		createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
//			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
//			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
//			stagingBuffer, stagingBufferMemory);
//		void* data;
//		vkMapMemory(vkDevice, stagingBufferMemory, 0, imageSize, 0, &data);
//		memcpy(data, pixels, static_cast<size_t>(imageSize));
//		vkUnmapMemory(vkDevice, stagingBufferMemory);
//
//		stbi_image_free(pixels);
//
//		createImage(texWidth, texHeight, mipLevels, VK_FORMAT_R8G8B8A8_SRGB,
//			VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
//			VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
//			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage,
//			textureImageMemory);
//
//		transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB,
//			VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels);
//		copyBufferToImage(stagingBuffer, textureImage,
//			static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
//
//		generateMipmaps(textureImage, VK_FORMAT_R8G8B8A8_SRGB,
//			texWidth, texHeight, mipLevels);
//
//		vkDestroyBuffer(device, stagingBuffer, nullptr);
//		vkFreeMemory(device, stagingBufferMemory, nullptr);
//		*/
//	}
//
//	// Here you load and setup all your Vulkan objects
//	// Etion :: probably to move inside the graphics engine
//	void Modeler::localInit() {
//		// Descriptor Layouts [what will be passed to the shaders]
//		/*
//		DSL1.init(this, {
//			// this array contains the binding:
//			// first  element : the binding number
//			// second element : the time of element (buffer or texture)
//			// third  element : the pipeline stage where it will be used
//			{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT},
//			{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
//			});
//
//		// Pipelines [Shader couples]
//		// The last array, is a vector of pointer to the layouts of the sets that will
//		// be used in this pipeline. The first element will be set 0, and so on..
//		P1.init(this, "shaders/vert.spv", "shaders/frag.spv", { &DSL1 });
//
//		// Models, textures and Descriptors (values assigned to the uniforms)
//		M1.init(this, MODEL_PATH);
//		T1.init(this, TEXTURE_PATH);
//		DS1.init(this, &DSL1, {
//			// the second parameter, is a pointer to the Uniform Set Layout of this set
//			// the last parameter is an array, with one element per binding of the set.
//			// first  elmenet : the binding number
//			// second element : UNIFORM or TEXTURE (an enum) depending on the type
//			// third  element : only for UNIFORMs, the size of the corresponding C++ object
//			// fourth element : only for TEXTUREs, the pointer to the corresponding texture object
//						{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
//						{1, TEXTURE, 0, &T1}
//			});
//			*/
//	}
//		
//		
//
//	// Here you destroy all the objects you created!	
//	// 
//	void Modeler::localCleanup() {
//		// cleanup the descriptor set,
//		// the texture,
//		// the model,
//		// the Pipeleine [Shader couples]
//		/*
//		DS1.cleanup();
//		T1.cleanup();
//		M1.cleanup();
//		P1.cleanup();
//		DSL1.cleanup();*/
//	}
//
//	// Here it is the creation of the command buffer:
//	// You send to the GPU all the objects you want to draw,
//	// with their buffers and textures
//	void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {
//		/*
//		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
//			P1.graphicsPipeline);
//
//		VkBuffer vertexBuffers[] = { M1.vertexBuffer };
//		// property .vertexBuffer of models, contains the VkBuffer handle to its vertex buffer
//		VkDeviceSize offsets[] = { 0 };
//		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
//		// property .indexBuffer of models, contains the VkBuffer handle to its index buffer
//		vkCmdBindIndexBuffer(commandBuffer, M1.indexBuffer, 0,
//			VK_INDEX_TYPE_UINT32);
//
//		// property .pipelineLayout of a pipeline contains its layout.
//		// property .descriptorSets of a descriptor set contains its elements.
//		vkCmdBindDescriptorSets(commandBuffer,
//			VK_PIPELINE_BIND_POINT_GRAPHICS,
//			P1.pipelineLayout, 0, 1, &DS1.descriptorSets[currentImage],
//			0, nullptr);
//
//		// property .indices.size() of models, contains the number of triangles * 3 of the mesh.
//		vkCmdDrawIndexed(commandBuffer,
//			static_cast<uint32_t>(M1.indices.size()), 1, 0, 0, 0);
//		*/
//	}
//
//	// Here is where you update the uniforms.
//	// Very likely this will be where you will be writing the logic of your application.
//	// Etion :: Very likely to be moved to the graphics engine.
//	void updateUniformBuffer(uint32_t currentImage) {
//		/*
//		static auto startTime = std::chrono::high_resolution_clock::now();
//		auto currentTime = std::chrono::high_resolution_clock::now();
//		float time = std::chrono::duration<float, std::chrono::seconds::period>
//			(currentTime - startTime).count();
//
//
//		UniformBufferObject ubo{};
//		ubo.model = glm::rotate(glm::mat4(1.0f),
//			time * glm::radians(90.0f),
//			glm::vec3(0.0f, 0.0f, 1.0f));
//		ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f),
//			glm::vec3(0.0f, 0.0f, 0.0f),
//			glm::vec3(0.0f, 0.0f, 1.0f));
//		ubo.proj = glm::perspective(glm::radians(45.0f),
//			swapChainExtent.width / (float)swapChainExtent.height,
//			0.1f, 10.0f);
//		ubo.proj[1][1] *= -1;
//
//		void* data;
//
//		// Here is where you actually update your uniforms
//		vkMapMemory(device, DS1.uniformBuffersMemory[0][currentImage], 0,
//			sizeof(ubo), 0, &data);
//		memcpy(data, &ubo, sizeof(ubo));
//		vkUnmapMemory(device, DS1.uniformBuffersMemory[0][currentImage]);
//		*/
//	}
//}