// This has been adapted from the Vulkan tutorial

#include "MyProject.hpp"
#include "map";
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

	//			GRAPHICS ENGINE OBJECT INITIALIZATION
	// not important for game design, part of graphics.
	// 
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
		std::vector<std::string> names;
		int numInstances = 1;
	};
	struct objectInitializer {
		Paths paths;
		Model model;
		Texture texture;
	};
	std::vector<objectInitializer> listOfObjectInitializers;
	std::map<std::string, DescriptorSet> descriptorSetMap;



	// Begin :				GAME DESIGN Information
	// INFORMATION USED FOR WHICH OBJECTS TO CREATE ON SCREEN
	std::vector<Paths> listOfPaths;
	struct objectDescriptor {
		// float acceleration;
		// other properties like velocity;
		glm::mat4 coordinates;
		float velocity = 1;
	};
	// key-value pair with key := name of object and objectDescriptor containing relevant 
	// mat4 passed to vertex and frag shader
	std::map<std::string, objectDescriptor> mapOfObjects;
	

	// Here you set the main application parameters
	void setWindowParameters() {
		// window size, titile and initial background
		{
			windowWidth = 800;
			windowHeight = 600;
			windowTitle = "My Project";
			initialBackgroundColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		}

		// HERE: We put all objects' texture and model paths, and we give them a list of names we
		// want the objects to be called
		// put all this into a function called loadAllTextures //
		{
			listOfPaths = std::vector<Paths>();
			listOfObjectInitializers = std::vector<objectInitializer>();
			std::vector bird_name = std::vector<std::string>();
			bird_name.push_back("main-bird");
			Paths bird_paths = Paths{ MODEL_PATH, TEXTURE_PATH , bird_name };
			std::vector bg_name = std::vector<std::string>();
			bg_name.push_back("background");
			Paths prof_paths = Paths{ PROF_MODEL, PROF_TEXTURE  , bg_name };
			std::vector all_names = std::vector<std::string>();
			all_names.insert(all_names.end(), { "small-bird-one", "small-bird-two", "small-bird-three" });
			Paths many_instances_obj_path = Paths{ MODEL_PATH, TEXTURE_PATH, all_names, 3 };


			listOfPaths.push_back(bird_paths);
			listOfPaths.push_back(prof_paths);
			listOfPaths.push_back(many_instances_obj_path);
		}
		
		// Do not change
		{
			int numOfAssets = 0;
			for (Paths paths : listOfPaths) {
				numOfAssets += paths.numInstances;
			}

			// Plus one because one is the camera DS.
			uniformBlocksInPool = 1 + numOfAssets;
			setsInPool = 1 + numOfAssets;
			// formula of this field is like above without + 1. This because the camera has no texture
			texturesInPool = listOfPaths.size();
		}
	}
	// END :					GAME DESIGN INFORMATION

	// Initialization of all textures, geometrical models and descriptor sets
	void localInit() {
		DSLobj.init(this, {
			{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT},
			{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
			});
		DSLglobal.init(this, {
			{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},
			});
		// in pipeline we receive in set 0 the global view and in set 1 the ubo
		P1.init(this, "shaders/vert.spv", "shaders/frag.spv", { &DSLglobal, &DSLobj });
		descriptorSetMap = std::map<std::string, DescriptorSet>();
		for (Paths objPaths : listOfPaths) {
			Model model = Model();
			Texture texture = Texture();

			model.init(this, objPaths.model_path);
			texture.init(this, objPaths.texture_path);
			std::map<std::string,DescriptorSet> dsmap = std::map<std::string, DescriptorSet>();
			for (int i = 0; i < objPaths.numInstances; i++) {
				objectDescriptor objectDescription = objectDescriptor{ 
													getInitialPosition(objPaths.names[i]) };
				DescriptorSet ds = DescriptorSet();
				ds.init(this, &DSLobj, {
								{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
								{1, TEXTURE, 0, &(texture)},
				});

				// Here we can get the initial position of each object
				//std::cout << "\n\n" << objPaths.names[i] << "\n\n";
				mapOfObjects.insert({ objPaths.names[i], objectDescription });
				dsmap.insert({ objPaths.names[i], ds });
				descriptorSetMap.insert({ objPaths.names[i], ds });
			}
			objectInitializer myObject = objectInitializer{ objPaths, model, texture };
			listOfObjectInitializers.push_back(myObject);
		}
		// after everything is done instance a Descriptor set for the global view
		DSglobal.init(this, &DSLglobal, {{0, UNIFORM, sizeof(UniformBufferObject), nullptr}});

	}

	void localCleanup() {
		for (objectInitializer objStarter : listOfObjectInitializers) {
			// clean up of all descriptor sets in the map
			//for (auto itr = objStarter.descriptorSetMap.begin(); itr != objStarter.descriptorSetMap.end(); ++itr) {
			//	// itr->first is the key
			//	itr->second.cleanup();
			//}
			objStarter.texture.cleanup();
			objStarter.model.cleanup();
		}
		for (auto itr = descriptorSetMap.begin(); itr != descriptorSetMap.end(); ++itr) {
			// itr->first is the key
			itr->second.cleanup();
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
		for (objectInitializer objStarter : listOfObjectInitializers) {
			VkBuffer vertexBuffers[] = { objStarter.model.vertexBuffer };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
			vkCmdBindIndexBuffer(commandBuffer, objStarter.model.indexBuffer, 0,
				VK_INDEX_TYPE_UINT32);
			for (int i = 0; i < objStarter.paths.numInstances; i++) {
				/// iff we have a vector of descriptor sets.
				// itr->second is the descriptorMap
				DescriptorSet dedicatedDS = descriptorSetMap[objStarter.paths.names[i]];
				vkCmdBindDescriptorSets(commandBuffer,
					VK_PIPELINE_BIND_POINT_GRAPHICS,
					P1.pipelineLayout, 1, 1,
					&(dedicatedDS.descriptorSets[currentImage]),
					0, nullptr);

				vkCmdDrawIndexed(commandBuffer,
					static_cast<uint32_t>(objStarter.model.indices.size()), 1, 0, 0, 0);
			}
			
		}
	}

	// Main functoin that gets called to draw on screen everything
	void updateUniformBuffer(uint32_t currentImage) {
		float time;
		// Get time
		{
		static auto startTime = std::chrono::high_resolution_clock::now();
		auto currentTime = std::chrono::high_resolution_clock::now();
		time = std::chrono::duration<float, std::chrono::seconds::period>
			(currentTime - startTime).count();
		}
		// These can be put inside the main loop
		handleKeyPresses(time);
		handleAutomaticObjectMovement(time);
		GlobalUniformBufferObject gubo = cameraTransformations();
		// Here the model of each object is sent to the shaders.
		{
			
			void* data;
			{
				vkMapMemory(device, DSglobal.uniformBuffersMemory[0][currentImage], 0,
					sizeof(gubo), 0, &data);
				memcpy(data, &gubo, sizeof(gubo));
				vkUnmapMemory(device, DSglobal.uniformBuffersMemory[0][currentImage]);
			}

			UniformBufferObject ubo{};
			for (auto itr = mapOfObjects.begin(); itr != mapOfObjects.end(); ++itr) {
				// itr->first has the key of the map i.e., the name of this object
				// itr->second has the value we want of the obj and its details
				objectDescriptor obj = itr->second;
				std::string obj_key = itr->first;
				// assign the mat4 coordinates to this object that  
				// will be sent to the vertex and frag shader
				DescriptorSet obj_descriptor_set = descriptorSetMap[obj_key];
				ubo.model = obj.coordinates;
				vkMapMemory(device, obj_descriptor_set.uniformBuffersMemory[0][currentImage], 0,
					sizeof(ubo), 0, &data);
				memcpy(data, &ubo, sizeof(ubo));
				vkUnmapMemory(device, obj_descriptor_set.uniformBuffersMemory[0][currentImage]);
			}
		}
		
	
	}
	// API (or method calls rather) to be used by the physics engine or game engine IDK.
	// sets the initial position on the scene of each object 
	glm::mat4 getInitialPosition(std::string objName) {
		if (objName.compare("background")) {
			glm::mat4 scaledUpHouse = glm::scale(glm::translate(glm::mat4(1.0f),
				glm::vec3(0.f, -2.f, 1.5f)), glm::vec3(6.5f));
			return glm::rotate(scaledUpHouse, glm::radians(90.0f),
				glm::vec3(-1.0f, 0.0f, 0.0f));
		}
		// we are in C++17 so we do not have the method contains... why is cpp like this?
		else if (objName.find("small-bird") != std::string::npos) {
			return glm::mat4(2.0f);
		}
		return glm::mat4(1.2f);
	}
	
	// controls the movement of objects inside the scene that can be controlled by the user
	void handleKeyPresses(float time) {

		glm::mat4 position = glm::translate(glm::mat4(1.0f), glm::vec3(0.f, 1.f, 0.f));
		glm::mat4 finalPosition;
		std::string main_object_name = "main-bird";
		glm::mat4 currentPosition = mapOfObjects[main_object_name].coordinates;
		float x = 0, y = 0, z = 0;

		if (glfwGetKey(window, GLFW_KEY_W)) {
			y = mapOfObjects[main_object_name].velocity;
		}
		if (glfwGetKey(window, GLFW_KEY_A)) {
			x = mapOfObjects[main_object_name].velocity;
		}
		if (glfwGetKey(window, GLFW_KEY_S)) {
			y = - mapOfObjects[main_object_name].velocity;
		}
		if (glfwGetKey(window, GLFW_KEY_D)) {
			x = - mapOfObjects[main_object_name].velocity;
		}
		if (glfwGetKey(window, GLFW_KEY_R)) {
			z = mapOfObjects[main_object_name].velocity;
		}
		if (glfwGetKey(window, GLFW_KEY_F)) {
			z = -mapOfObjects[main_object_name].velocity;
		}
		float timeInMS = time / 1000 / 10;
		x = x * timeInMS;
		y = y * timeInMS;
		z = z * timeInMS;
		if (glfwGetKey(window, GLFW_KEY_SPACE)) {
			finalPosition = glm::scale(position, glm::vec3(2.20f));
		}
		else {
			finalPosition = glm::translate(currentPosition, glm::vec3(x, y, z));
		}
		mapOfObjects[main_object_name].coordinates = finalPosition;

	}
	
	// Controls the camera movement
	GlobalUniformBufferObject cameraTransformations() {
		GlobalUniformBufferObject gubo{};
		gubo.view = glm::lookAt(glm::vec3(0.0f, 1.0f, -6.5f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f));
		gubo.proj = glm::perspective(glm::radians(60.0f),
			swapChainExtent.width / (float)swapChainExtent.height,
			0.1f, 100.0f);
		gubo.proj[1][1] *= -1;
		return gubo;
	}

	// controls the movement of objects inside the scene that cannot be controlled by the user
	void handleAutomaticObjectMovement(float time) {
		// TODO:: Use the initial position
		glm::mat4 transMat4 = glm::translate(glm::mat4(1.0f), glm::vec3(0.f, 1.f, 0.f));
		glm::mat4 scaledMat4;
		glm::mat4 finalMat4;
		for (auto itr = mapOfObjects.begin(); itr != mapOfObjects.end(); ++itr) {
			if (itr->first.compare("background") == 0) {
				// TODO:: understand why we need to rotate 90 degrees.
				transMat4 = glm::translate(glm::mat4(1.0f), glm::vec3(0.f, -1.f, 1.5f));
				scaledMat4 = glm::scale(transMat4, glm::vec3(4.5f));
				finalMat4 = glm::rotate(scaledMat4, glm::radians(90.0f),
					glm::vec3(-1.0f, 0.0f, 0.0f));
			}
			else if (itr->first.find("small-bird-one") != std::string::npos) {
				transMat4 = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 1.8f));
				scaledMat4 = glm::scale(transMat4, glm::vec3(0.68f));
				finalMat4 = glm::rotate(scaledMat4,
					time / 3 * glm::radians(90.0f),
					glm::vec3(0.0f, 1.0f, 0.0f));
			}
			else if (itr->first.find("small-bird-two") != std::string::npos) {
				transMat4 = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, -0.5f, 1.2f));
				scaledMat4 = glm::scale(transMat4, glm::vec3(0.38f));
				finalMat4 = glm::rotate(scaledMat4,
					time / 2 * glm::radians(90.0f),
					glm::vec3(1.0f, 0.0f, 0.0f));
			}
			else if (itr->first.find("small-bird-three") != std::string::npos) {
				transMat4 = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, 0.5f, 1.2f));
				scaledMat4 = glm::scale(transMat4, glm::vec3(0.38f));
				finalMat4 = glm::rotate(scaledMat4,
					time * glm::radians(90.0f),
					glm::vec3(0.0f, 1.0f, 1.0f));
			}
			// Main bird is supposed to be the main playable character
			if (itr->first.compare("main-bird") != 0) {
				itr->second.coordinates = finalMat4;
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
