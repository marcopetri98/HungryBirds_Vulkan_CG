// This has been adapted from the Vulkan tutorial

#include "MyProject.hpp"
#include "map";
using Clock = std::chrono::high_resolution_clock;
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
	alignas(128) glm::mat4 model;
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
	Pipeline P_Sky;
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
	float angle1 = 0.f;
	float angle2 = 0.f;
	bool launch = false;
	std::string lastCollision="None";
	float t = 0.f;
	std::chrono::time_point<Clock> launchTime;
	float module = 50.f;
	glm::mat4 collisionPoint = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, 0.f));


	// Begin :				GAME DESIGN Information
	// INFORMATION USED FOR WHICH OBJECTS TO CREATE ON SCREEN
	std::vector<Paths> listOfPaths;
	struct objectDescriptor {
		// float acceleration;
		// other properties like velocity;
		glm::mat4 coordinates;
		int colliderType; //TODO use enum to represent e.g. box, sphere...
		float colliderRadius;
		float velocity = 1;
	};
	// key-value pair with key := name of object and objectDescriptor containing relevant 
	// mat4 passed to vertex and frag shader
	std::map<std::string, objectDescriptor> mapOfObjects;
	struct guboWrapper {
		GlobalUniformBufferObject gubo;
		glm::mat4 coordinates;
		int position;
		bool anchored = false;
	};
	const int uninitializedFlag = -10101;
	guboWrapper camera{
		{},
		{},
		uninitializedFlag,
		true
	};
	

	// Here you set the main application parameters
	void setWindowParameters() {
		// window size, titile and initial background
		{
			windowWidth = 1200;
			windowHeight = 720;
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
			std::vector background_plane = std::vector<std::string>();
			background_plane.push_back("background");
			//Paths background_plane_paths = Paths{ "models/planar-background.obj",
			//									"textures/background-example.jpg",
			//									background_plane };
			Paths background_plane_paths = Paths{ "models/skybox.obj",
												"textures/skybox-texture.png",
												background_plane };
			std::vector background_skyline_names = std::vector<std::string>();
			background_skyline_names.push_back("background-skyline");
			Paths background_skyline = Paths{ "models/MountainSkybox.obj",
												"textures/SkyBoxes/mountain.png",
												background_skyline_names
			};
			std::vector all_names = std::vector<std::string>();
			all_names.insert(all_names.end(), { "small-bird-one", "small-bird-two", "small-bird-three" });
			Paths many_instances_obj_path = Paths{ MODEL_PATH, TEXTURE_PATH, all_names, 3 };
			std::vector arrow_name = std::vector<std::string>();
			arrow_name.push_back("arrow");
			Paths arrow_paths = Paths{ "models/arrow.obj", "textures/arrow.png", arrow_name};
			
			std::vector cube_name = std::vector<std::string>();
			cube_name.push_back("cube");
			Paths cube_paths = Paths{ "models/cube.obj", "textures/cube.png", cube_name};

			listOfPaths.push_back(background_plane_paths);
			listOfPaths.push_back(background_skyline);
			listOfPaths.push_back(bird_paths);
			listOfPaths.push_back(arrow_paths);
			listOfPaths.push_back(many_instances_obj_path);
			listOfPaths.push_back(cube_paths);
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
		P_Sky.init(this, "shaders/vert.spv", "shaders/frag_sky.spv", { &DSLglobal, &DSLobj });
		descriptorSetMap = std::map<std::string, DescriptorSet>();
		for (Paths objPaths : listOfPaths) {
			Model model = Model();
			Texture texture = Texture();

			model.init(this, objPaths.model_path);
			texture.init(this, objPaths.texture_path);
			std::map<std::string,DescriptorSet> dsmap = std::map<std::string, DescriptorSet>();
			for (int i = 0; i < objPaths.numInstances; i++) {
				objectDescriptor objectDescription = objectDescriptor{ 
													getInitialPosition(objPaths.names[i]), 0, 1.0f };
				DescriptorSet ds = DescriptorSet();
				ds.init(this, &DSLobj, {
								{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
								{1, TEXTURE, 0, &(texture)},
				});

				// Here we can get the initial position of each object
				//std::cout << "\n\n" << objPaths.names[i] << "\n\n";
				mapOfObjects.insert({ objPaths.names[i], objectDescription});
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
		P_Sky.cleanup();
		DSLglobal.cleanup();
		DSLobj.cleanup();
		DSglobal.cleanup();

	}

	void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {
		// Binding different pipelines (for skybox is different and for other objects is different)
		{
			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
				P1.graphicsPipeline);
			vkCmdBindDescriptorSets(commandBuffer,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				P1.pipelineLayout, 
				// global view camera is bound in set 0 
				0,
				1, &(DSglobal.descriptorSets[currentImage]),
				0, nullptr);
			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
				P_Sky.graphicsPipeline);
			vkCmdBindDescriptorSets(commandBuffer,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				P_Sky.pipelineLayout,
				// global view camera is bound in set 0 
				0,
				1, &(DSglobal.descriptorSets[currentImage]),
				0, nullptr);
		}
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
				if (objStarter.paths.names[i].find("background") == std::string::npos) {
					printf("Is not background\n");
					vkCmdBindDescriptorSets(commandBuffer,
						VK_PIPELINE_BIND_POINT_GRAPHICS,
						P1.pipelineLayout, 1, 1,
						&(dedicatedDS.descriptorSets[currentImage]),
						0, nullptr);
				}
				else {
					printf("Is background\n");
					vkCmdBindDescriptorSets(commandBuffer,
						VK_PIPELINE_BIND_POINT_GRAPHICS,
						P_Sky.pipelineLayout, 1, 1,
						&(dedicatedDS.descriptorSets[currentImage]),
						0, nullptr);
				}
				

				vkCmdDrawIndexed(commandBuffer,
					static_cast<uint32_t>(objStarter.model.indices.size()), 1, 0, 0, 0);
			}
			
		}
	}

	// Main functoin that gets called to draw on screen everything
	void updateUniformBuffer(uint32_t currentImage) {
		float time;
		float deltaTime;
		// Get time
		{
		static auto startTime= std::chrono::high_resolution_clock::now();
		static auto lastTime = startTime;
		auto currentTime = std::chrono::high_resolution_clock::now();
		time = std::chrono::duration<float, std::chrono::seconds::period>
			(currentTime - startTime).count();
		deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastTime).count();
		lastTime = currentTime;
		}
		// These can be put inside the main loop
		handleKeyPresses(time);
		handleAutomaticObjectMovement(time);
		GlobalUniformBufferObject gubo = cameraTransformations(time, deltaTime);
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
		// we are in C++17 so we do not have the method contains... why is cpp like this?
		if (objName.find("small-bird") != std::string::npos) {
			return glm::mat4(10.0f);
		}
		else if (!objName.compare("main-bird")) {
			return glm::mat4(1.2f);
		}
		else if (!objName.compare("arrow")) {
			glm::mat4 transl = glm::translate(glm::mat4(1.f), glm::vec3(.4f, 1.2f, 17.f));
			return glm::rotate(transl, glm::radians(270.0f),
				glm::vec3(0.0f, 1.0f, 0.0f));
		}
		else if (!objName.compare("cube")) {
			glm::mat4 transl = glm::translate(glm::mat4(1.f), glm::vec3(1.2f, 5.f, 50.f));
			return glm::scale(transl, glm::vec3(15.f));
		}
		else if (objName.find("background") != std::string::npos) {
			if (camera.position == uninitializedFlag) {
				cameraTransformations(0, 0);
			}
			glm::mat4 scaled_up;
			// if not background
			if (objName.compare("background")) {
				scaled_up = glm::translate(
					glm::scale(camera.gubo.view, glm::vec3(4000.f)),
					glm::vec3(0.f, -0.15f, 0.f));
			}
			else {
				scaled_up = glm::translate(
					glm::scale(camera.gubo.view, glm::vec3(8000.f)),
					glm::vec3(0.f, -0.15f, 0.f));
			}
			
			return scaled_up;
			
		}
		return glm::mat4(1.2f);
	}
	
	// controls the movement of objects inside the scene that can be controlled by the user
	void handleKeyPresses(float time) {
		glm::mat4 position = glm::translate(glm::mat4(1.0f), glm::vec3(0.f, 1.f, 0.f));
		glm::mat4 finalPosition;
		std::string main_object_name = "arrow";
		glm::mat4 currentPosition = mapOfObjects[main_object_name].coordinates;
		float x = 0, y = 0, z = 0;
		float timeInMS = time / 1000 / 10;
		if (!launch) {
			if (glfwGetKey(window, GLFW_KEY_W)) {
				angle1 -= 0.01f;
			}
			if (glfwGetKey(window, GLFW_KEY_A)) {
				angle2 += 0.01f;
			}
			if (glfwGetKey(window, GLFW_KEY_S)) {
				angle1 += 0.01f;
			}
			if (glfwGetKey(window, GLFW_KEY_D)) {
				angle2 -= 0.01f;
			}
			if (angle1 >= 0) {
				angle1 = 0;
			}
			if (angle1 <= -25) {
				angle1 = -25;
			}
			if (angle2 >= 25) {
				angle2 = 25;
			}
			if (angle2 <= -25) {
				angle2 = -25;
			}
			if (glfwGetKey(window, GLFW_KEY_SPACE)) {
				launch = true;
				launchTime = std::chrono::high_resolution_clock::now();
			}
			glm::mat4 initial = getInitialPosition(main_object_name);
			glm::mat4 T = glm::translate(glm::mat4(1), glm::vec3(0.3f, 1.2f, -2.f));
			glm::mat4 r1 = glm::rotate(glm::mat4(1), glm::radians(angle1), glm::vec3(1, 0, 0));
			glm::mat4 r2 = glm::rotate(glm::mat4(1), glm::radians(angle2), glm::vec3(0, 1, 0));
			finalPosition = T * r2 * r1 * glm::inverse(T) * initial;
		}
		else {
			if (glfwGetKey(window, GLFW_KEY_R)) {
				launch = false;
				mapOfObjects["main-bird"].coordinates = getInitialPosition("main-bird");
			}
			finalPosition = glm::mat4(-500.f);
		}
		mapOfObjects[main_object_name].coordinates = finalPosition;
		return;

		if (glfwGetKey(window, GLFW_KEY_R)) {
			z = mapOfObjects[main_object_name].velocity;
		}
		if (glfwGetKey(window, GLFW_KEY_F)) {
			z = -mapOfObjects[main_object_name].velocity;
		}
		x = x * timeInMS;
		y = y * timeInMS;
		z = z * timeInMS;
		if (glfwGetKey(window, GLFW_KEY_SPACE)) {
			finalPosition = glm::scale(position, glm::vec3(0.5f));
		}
		else {
			finalPosition = glm::translate(currentPosition, glm::vec3(x, y, z));
		}
		mapOfObjects[main_object_name].coordinates = finalPosition;

	}
	
	// Controls the camera movement
	GlobalUniformBufferObject cameraTransformations(float time,float deltaTime) {
		
		GlobalUniformBufferObject gubo{};
		if (camera.position == uninitializedFlag) {
			camera.position = 1;
			camera.anchored = false;
			camera.gubo.view = glm::lookAt(glm::vec3(0.f, 10.0f, -100.0f),
				glm::vec3(0.0f, 0.0f, 0.0f),
				glm::vec3(0.0f, 1.0f, 0.0f));
		}
		static float debounce = time;
		if (glfwGetKey(window, GLFW_KEY_C)) {
			if (time - debounce > 0.33) {
				camera.anchored = true;
				debounce = time;
			}			
		}

		moveCamera(deltaTime);

		gubo.proj = glm::perspective(glm::radians(60.0f),
			swapChainExtent.width / (float)swapChainExtent.height,
			0.1f, 10000.f);
		gubo.proj[1][1] *= -1;
		camera.gubo.proj = gubo.proj;
		return camera.gubo;
	}

	void moveCamera(float deltaTime) {
		GlobalUniformBufferObject gubo{};
		gubo.view = camera.gubo.view;

		if (camera.anchored && camera.position == 0) {
			// Position 1
			gubo.view = glm::lookAt(glm::vec3(0.f, 10.0f, -100.0f),
				glm::vec3(0.0f, 0.0f, 0.0f),
				glm::vec3(0.0f, 1.0f, 0.0f));
			camera.position = 1;
			camera.anchored = false;
		}
		else if (camera.anchored && camera.position == 1) {

			// Position 0
			gubo.view = glm::lookAt(glm::vec3(100.f, 0.0f, 0.0f),
				glm::vec3(25.0f, 0.0f, -2.0f),
				glm::vec3(0.0f, 1.0f, 0.0f));
			camera.position = 0;
			camera.anchored = false;

		}
		else {

			float speed = 19.89;
			float deltaT = deltaTime * speed;

			glm::mat4 _skyboxScaledDown = glm::scale(
											glm::translate(mapOfObjects["background"].coordinates, glm::vec3(0.f, 0.15f, 0.f)),
											glm::vec3(1/8000.f));

			int leftOrRight = ( camera.position);
			int upOrDown = (1 - camera.position);
			if (glfwGetKey(window, GLFW_KEY_UP)) {
				gubo.view = glm::translate(gubo.view, glm::vec3(0, -deltaT, 0));
			}
			else if (glfwGetKey(window, GLFW_KEY_DOWN)) {
				gubo.view = glm::translate(gubo.view, glm::vec3(0, deltaT, 0));
			}
			if (glfwGetKey(window, GLFW_KEY_LEFT)) {
				gubo.view = glm::translate(gubo.view, glm::vec3(-deltaT * leftOrRight, 0, -upOrDown*deltaT));
			}
			else if (glfwGetKey(window, GLFW_KEY_RIGHT)) {
				gubo.view = glm::translate(gubo.view, glm::vec3(deltaT * leftOrRight, 0, upOrDown * deltaT));
			}

			/*if (glfwGetKey(window, GLFW_KEY_I)) {
				mapOfObjects["background"].coordinates =
					glm::translate(mapOfObjects["background"].coordinates, glm::vec3(0, 0, deltaT));
			}
			else if (glfwGetKey(window, GLFW_KEY_K)) {
				mapOfObjects["background"].coordinates =
					glm::translate(mapOfObjects["background"].coordinates, glm::vec3(0, 0, -deltaT));
			}*/
			_skyboxScaledDown = gubo.view;
			mapOfObjects["background"].coordinates = glm::translate(glm::scale(_skyboxScaledDown, 
				glm::vec3(8000.f)), glm::vec3(-0.f, -0.15f, 0.f));

		}
		camera.anchored = false;
		camera.gubo.view = gubo.view;
	}

	// controls the movement of objects inside the scene that cannot be controlled by the user
	void handleAutomaticObjectMovement(float time) {
		// TODO:: Use the initial position
		glm::mat4 transMat4 = glm::translate(glm::mat4(1.0f), glm::vec3(0.f, 1.f, 0.f));
		glm::mat4 scaledMat4;
		glm::mat4 finalMat4;
		for (auto itr = mapOfObjects.begin(); itr != mapOfObjects.end(); ++itr) {
			if (itr->first.find("background") != std::string::npos) {
				finalMat4 = itr->second.coordinates;
			}
			if (itr->first.compare("cube") == 0) {
				finalMat4 = itr->second.coordinates;
			}
			else if (itr->first.find("main-bird") != std::string::npos) {
				transMat4 = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, -4.0f));
				scaledMat4 = glm::scale(transMat4, glm::vec3(2.0f));
				finalMat4 = glm::rotate(scaledMat4,
					glm::radians(180.0f),
					glm::vec3(0.0f, 1.0f, 0.0f));
				if (launch)
				{
					auto currentTime = std::chrono::high_resolution_clock::now();
					float lTime = std::chrono::duration<float, std::chrono::seconds::period>
						(currentTime - launchTime).count();
					float g = 9.81f;
					float angle = -angle1;
					glm::mat4 T = glm::translate(glm::mat4(1), glm::vec3(1.2f));
					glm::mat4 R = glm::rotate(glm::mat4(1), glm::radians(angle2), glm::vec3(0, 1, 0));
					float zt = module * cos(glm::radians(angle)) * lTime;
					float yt = -1.*  1./2. * g * pow(lTime, 2.) + module * sin(glm::radians(angle)) * lTime;
					glm::mat4 posT = glm::translate(glm::mat4(1), glm::vec3(0.f, yt, zt));
					finalMat4 = collisionPoint * T * R * posT * glm::inverse(R) * glm::inverse(T)* finalMat4;

					float rayAngle = 0.f;
					while (rayAngle < 360.f) {
						//TODO for now 2d game, suppose x is constant
						float r = itr->second.colliderRadius + 2.f;
						glm::vec3 rayDir = glm::vec3(0, r * sin(glm::radians(rayAngle)), r * cos(glm::radians(rayAngle)));
						glm::vec3 rayPos = rayDir + glm::vec3(finalMat4[3][0], finalMat4[3][1], finalMat4[3][2]);
							for (auto itr2 = mapOfObjects.begin(); itr2 != mapOfObjects.end(); ++itr2) {
								if (itr2->first.find("small-bird") != std::string::npos || itr2->first.compare("main-bird") == 0 || itr2->first.compare("arrow") == 0 || itr2->first.compare("background") == 0)
									continue; 
								//TODO handle different types of colliders
								glm::mat4 pos = itr2->second.coordinates;
								float radius = itr2->second.colliderRadius;
								printf("pos: %f %f %f radius: % f rayPos: % f % f % f \n", 
									pos[3][0], pos[3][1], pos[3][2], radius, rayPos.x, rayPos.y, rayPos.z);
								if (pos[3][0] - radius <= rayPos.x && rayPos.x <= pos[3][0] + radius &&
									pos[3][1] - radius <= rayPos.y && rayPos.y <= pos[3][1] + radius &&
									pos[3][2] - radius <= rayPos.z && rayPos.z <= pos[3][2] + radius && itr2->first.compare(lastCollision)!=0) {
									std::cout << "collided on angle: " << rayAngle << " with object: " << itr2->first << "\n";
									launchTime = std::chrono::high_resolution_clock::now();
									module = 20.f;
									angle1 = rayAngle-180;
									rayAngle = 360;
									lastCollision = itr->first;
									collisionPoint = glm::translate(glm::mat4(1), glm::vec3(pos[3][0], pos[3][1], pos[3][2]));
									break;
								}
							}
							transMat4 = glm::translate(glm::mat4(1.0f), rayPos);
							scaledMat4 = glm::scale(transMat4, glm::vec3(0.5f));
							mapOfObjects["small-bird-two"].coordinates = scaledMat4;
							printf("%f %f %f \n", 
								mapOfObjects["small-bird-two"].coordinates[3][0],
								mapOfObjects["small-bird-two"].coordinates[3][1], 
								mapOfObjects["small-bird-two"].coordinates[3][2]);
							rayAngle += 45.f;
					}

				}
			}
			/*
			else if (itr->first.find("small-bird-two") != std::string::npos) {
				transMat4 = glm::translate(glm::mat4(1.0f), glm::vec3(0.f, 0.f, 0.f));
				scaledMat4 = glm::scale(transMat4, glm::vec3(1.f));
				finalMat4 = scaledMat4;
			}
			*/
			else if (itr->first.find("small-bird-three") != std::string::npos) {
				transMat4 = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, 0.5f, 1.2f));
				scaledMat4 = glm::scale(transMat4, glm::vec3(0.38f));
				finalMat4 = glm::rotate(scaledMat4,
					time * glm::radians(90.0f),
					glm::vec3(0.0f, 1.0f, 1.0f));
			}
			// Main bird is supposed to be the main playable character
			if (itr->first.compare("arrow") != 0 && itr->first.compare("small-bird-two") != 0) {
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
