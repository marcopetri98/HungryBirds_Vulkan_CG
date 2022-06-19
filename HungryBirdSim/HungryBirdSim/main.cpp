// This has been adapted from the Vulkan tutorial

#include "MyProject.hpp"
const std::string MY_MODEL = "models/birdhead.obj";
const std::string PROF_MODEL = "models/viking_room.obj";
const std::string MODEL_PATH = MY_MODEL;
const std::string MY_TEXTURE = "textures/birdheadTextHead.png";
const std::string PROF_TEXTURE = "textures/viking_room.png"; 
const std::string TEXTURE_PATH = MY_TEXTURE;
// POSITION OF THE CAMERA VIEW
struct GlobalUniformBufferObject {
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};
// The uniform buffer object used in this example
struct UniformBufferObject {
	alignas(16) glm::mat4 model;
};

// MAIN ! 
class MyProject : public BaseProject {
protected:
	// This describes the layout of the vertex buffer for the gubo object
	DescriptorSetLayout DSLglobal;
	// this is the instance of the global camera
	DescriptorSet DSglobal;
	
	// This describes the layout of the vertex buffer for each ubo object
	DescriptorSetLayout DSLobj;
	// This is the pipeline where all the commands are passed through
	Pipeline P1;
	struct Paths {
		std::string model_path;
		std::string texture_path;
		std::string name;
		int numInstances = 1;
	};
	struct objectInitializer {
		Paths paths;
		Model model = Model();
		Texture texture = Texture();
		DescriptorSet descriptorSet = DescriptorSet();
	};

	std::vector<objectInitializer> listOfObjects;
	std::vector<Paths> listOfPaths;

	// Here you set the main application parameters
	void setWindowParameters() {
		// window size, titile and initial background
		windowWidth = 800;
		windowHeight = 600;
		windowTitle = "My Project";
		initialBackgroundColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		listOfPaths = std::vector<Paths>();
		listOfObjects = std::vector<objectInitializer>();

		Paths bird_paths = Paths{ MODEL_PATH, TEXTURE_PATH };
		Paths prof_paths = Paths{ PROF_MODEL, PROF_TEXTURE  };
		listOfPaths.push_back(bird_paths);
		listOfPaths.push_back(prof_paths);

		// Plus one because one is the camera DS.
		// formula of the two fields below is : 1 + (numInstances in each Path)
		uniformBlocksInPool = 1 + listOfPaths.size();
		setsInPool = 1 + listOfPaths.size();
		// formula of this field is like above - 1. This because the camera has no texture
		texturesInPool = listOfPaths.size();
	}


	void localInit() {
		DSLobj.init(this, {
			// this array contains the binding:
			// first  element : the binding number
			// second element : the time of element (buffer or texture)
			// third  element : the pipeline stage where it will be used
			{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT},
			{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
			});
		DSLglobal.init(this, {
			{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},
			});
		// in pipeline we receive in set 0 the global view and in set 1 the ubo
		P1.init(this, "shaders/vert.spv", "shaders/frag.spv", { &DSLglobal, &DSLobj });
		
		// is of type objectInitializer
		for (Paths objPaths : listOfPaths) {
			Model model = Model();
			Texture texture = Texture();
			DescriptorSet ds = DescriptorSet(); // every ds is instance of dslOBJ

			model.init(this, objPaths.model_path);
			texture.init(this, objPaths.texture_path);

			for (int i = 0; i < objPaths.numInstances; i++) {
				// listOfDescriptorSets.push_back( the thing below ); 
				ds.init(this, &DSLobj, {
								{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
								{1, TEXTURE, 0, &(texture)},
					});
			}
			// below here we should put listOfDescriptorSets instead of ds           //|
			objectInitializer myObject = objectInitializer{ objPaths, model, texture, ds };
			listOfObjects.push_back(myObject);
		}

		// after everything is done instance a Descriptor set for the global view
		DSglobal.init(this, &DSLglobal, {{0, UNIFORM, sizeof(UniformBufferObject), nullptr}});

	}

	// Here you destroy all the objects you created!		
	void localCleanup() {
		for (objectInitializer objStarter : listOfObjects) {
			objStarter.descriptorSet.cleanup();
			objStarter.texture.cleanup();
			objStarter.model.cleanup();
		}
		//constant part
		P1.cleanup();
		DSLglobal.cleanup();
		DSLobj.cleanup();
		DSglobal.cleanup();

	}

	void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
			P1.graphicsPipeline);
		vkCmdBindDescriptorSets(commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			P1.pipelineLayout, 
			// global view camera is bound in set 0 
			0,
			1, &(DSglobal.descriptorSets[currentImage]),
			0, nullptr);
		for (objectInitializer objStarter : listOfObjects) {
			VkBuffer vertexBuffers[] = { objStarter.model.vertexBuffer };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
			vkCmdBindIndexBuffer(commandBuffer, objStarter.model.indexBuffer, 0,
				VK_INDEX_TYPE_UINT32);
			for (int i = 0; i < objStarter.paths.numInstances; i++) {
				/// TODO:: The descriptor set MUST BE a DIFFERENT one for each INSTANCE
				/// objStarter.descriptorSet is wrong but objStarter.descriptorSet[i] might be right
				/// iff we have a vector of descriptor sets.
				vkCmdBindDescriptorSets(commandBuffer,
					VK_PIPELINE_BIND_POINT_GRAPHICS,
					P1.pipelineLayout, 1, 1, &(objStarter.descriptorSet.descriptorSets[currentImage]),
					0, nullptr);
				vkCmdDrawIndexed(commandBuffer,
					static_cast<uint32_t>(objStarter.model.indices.size()), 1, 0, 0, 0);
			}
			
		}
		
	}

	void updateUniformBuffer(uint32_t currentImage) {
		static auto startTime = std::chrono::high_resolution_clock::now();
		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>
			(currentTime - startTime).count();
		glm::mat4 position = glm::translate(glm::mat4(1.0f), glm::vec3(0.f, 1.f, 0.f));
		bool keyPressed = false;
		glm::mat4 scaledDownBird;
		glm::mat4 rotatedBird;
		if (glfwGetKey(window, GLFW_KEY_SPACE)) {
			position = glm::translate(glm::mat4(1.0f), glm::vec3(0.f, 0.f, 0.5f));
			scaledDownBird = glm::scale(position, glm::vec3(2.0f));
			rotatedBird = scaledDownBird;
			keyPressed = true;
		}

		GlobalUniformBufferObject gubo{};

		gubo.view = glm::lookAt(glm::vec3(0.0f, 1.0f, -5.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f));
		gubo.proj = glm::perspective(glm::radians(40.0f),
			swapChainExtent.width / (float)swapChainExtent.height,
			0.1f, 10.0f);
		gubo.proj[1][1] *= -1;
		void* data;
		// Put space in device to insert data from the void pointer.
		vkMapMemory(device, DSglobal.uniformBuffersMemory[0][currentImage], 0,
			sizeof(gubo), 0, &data);
		// copy the gubo to the data pointer
		memcpy(data, &gubo, sizeof(gubo));
		vkUnmapMemory(device, DSglobal.uniformBuffersMemory[0][currentImage]);

		UniformBufferObject ubo{};
		for (objectInitializer objStarter : listOfObjects) {
			// physics(objStarter, ubo.model){
			//		example::
			//		if (objStarter.name = 'bird-0'){
			//			glm::translate(glm::mat4(1.0f), glm::vec3(1.0f,1.0f,0.f));
			//		}
			// }
			// CHANGE FOR EACH OBJECT
			// TODO:: create API for 3d Affine Transformations
			//glm::mat4 scaling = glm::mat4(1.0f);
			//glm::mat4 rotation = glm::mat4(1.0f);

			for (int i = 0; i < objStarter.paths.numInstances; i++) {

				if (objStarter.paths.model_path.compare(PROF_MODEL) == 0) {
					// TODO:: understand why we need to rotate 90 degrees.
					position = glm::translate(glm::mat4(1.0f), glm::vec3(0.f, -1.f, 1.5f));
					glm::mat4 scaledUpHouse = glm::scale(position, glm::vec3(4.5f));
					ubo.model = glm::rotate(scaledUpHouse, glm::radians(90.0f),
						glm::vec3(-1.0f, 0.0f, 0.0f));
				}
				else {
					if (!keyPressed) {
						position = glm::translate(glm::mat4(1.0f), glm::vec3(0.f, 0.f, 1.5f));
						scaledDownBird = glm::scale(position, glm::vec3(0.25f));
						rotatedBird = glm::rotate(scaledDownBird,
							time * glm::radians(90.0f),
							glm::vec3(0.0f, 1.0f, 0.0f));
					}
					ubo.model = rotatedBird;
				}
				/// TODO:: descriptorSet[i]
				vkMapMemory(device, objStarter.descriptorSet.uniformBuffersMemory[0][currentImage], 0,
					sizeof(ubo), 0, &data);
				memcpy(data, &ubo, sizeof(ubo));
				vkUnmapMemory(device, objStarter.descriptorSet.uniformBuffersMemory[0][currentImage]);

			}
		}

		
	}


};

// This is the main: probably you do not need to touch this!
int main() {
	MyProject app;

	try {
		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
