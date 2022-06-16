// This has been adapted from the Vulkan tutorial

#include "MyProject.hpp"
const std::string MY_MODEL = "models/birdhead.obj";
const std::string PROF_MODEL = "models/viking_room.obj";
const std::string MODEL_PATH = MY_MODEL;
const std::string MY_TEXTURE = "textures/birdheadTextHead.png";

const std::string PROF_TEXTURE = "textures/viking_room.png"; 
const std::string TEXTURE_PATH = MY_TEXTURE;

// The uniform buffer object used in this example
struct UniformBufferObject {
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};

// MAIN ! 
class MyProject : public BaseProject {
protected:

	// Descriptor Layouts [what will be passed to the shaders]
// Pipelines [Shader couples]
	DescriptorSetLayout DSL1;
	Pipeline P1;
	// Etion -- maybe remove dsl as it should be global
	// implement numInstances
	// maybe also add a ubo for each object
		class objectInitializer {
		public:
			objectInitializer(
				std::string model_path,
				std::string texture_path,
				Model model = Model(),
				Texture texture = Texture(),
				DescriptorSet descriptorSet = DescriptorSet(),
				int numInstances = 1)
			{
				this->model = model;
				this->texture = texture;
				this->descriptorSet = descriptorSet;
				this->model_path = model_path;
				this->texture_path = texture_path;
				this->numInstances = numInstances;
			};
			Model model;
			Texture texture;
			DescriptorSet descriptorSet;
			std::string model_path;
			std::string texture_path;
			int numInstances;
		};
	
	

	// DescriptorSetLayout DSL1;

	std::vector<objectInitializer> listOfObjects;

	// Here you set the main application parameters
	void setWindowParameters() {
		// window size, titile and initial background
		windowWidth = 800;
		windowHeight = 600;
		windowTitle = "My Project";
		initialBackgroundColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		listOfObjects = std::vector<objectInitializer>();
		objectInitializer myObject = objectInitializer(MODEL_PATH, TEXTURE_PATH);
		
		listOfObjects.push_back(myObject);
		// Descriptor pool sizes
		std::cout << "\n Size ::: " << listOfObjects.size() << "\n";
		uniformBlocksInPool = listOfObjects.size();
		texturesInPool = listOfObjects.size();
		// Etion idk what this is
		setsInPool = listOfObjects.size();
	}

	// Here you load and setup all your Vulkan objects
	void localInit() {
		std::cout << "\nLOCAL INIT\n\n";
		DSL1.init(this, {
			// this array contains the binding:
			// first  element : the binding number
			// second element : the time of element (buffer or texture)
			// third  element : the pipeline stage where it will be used
			{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT},
			{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
			});
		P1.init(this, "shaders/vert.spv", "shaders/frag.spv", { &DSL1 });

		// is of type objectInitializer
		for (objectInitializer objStarter : listOfObjects) {

			objStarter.model.init(this, objStarter.model_path);
			objStarter.texture.init(this, objStarter.texture_path);
			objStarter.descriptorSet.init(this, &DSL1, {
				// the second parameter, is a pointer to the Uniform Set Layout of this set
				// the last parameter is an array, with one element per binding of the set.
				// first  elmenet : the binding number
				// second element : UNIFORM or TEXTURE (an enum) depending on the type
				// third  element : only for UNIFORMs, the size of the corresponding C++ object
				// fourth element : only for TEXTUREs, the pointer to the corresponding texture object
							{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
							{1, TEXTURE, 0, &objStarter.texture}
				});

		}
	}
	Pipeline PP;
	// Here you destroy all the objects you created!		
	void localCleanup() {
		P1.cleanup();
		DSL1.cleanup();
		for (objectInitializer objStarter : listOfObjects) {
			objStarter.descriptorSet.cleanup();
			objStarter.texture.cleanup();
			objStarter.model.cleanup();
		}
	}

	void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
			P1.graphicsPipeline);
		std::cout << "\npopulateCommandBuffer\n\n";
		for (objectInitializer objStarter : listOfObjects) {
			

			std::cout << "\nkCmdBindPipeline\n\n";
			VkBuffer vertexBuffers[] = { objStarter.model.vertexBuffer };
			// property .vertexBuffer of models, contains the VkBuffer handle to its vertex buffer
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
			std::cout << "\nkCmdBindVertexBuffers\n\n";
			// property .indexBuffer of models, contains the VkBuffer handle to its index buffer
			vkCmdBindIndexBuffer(commandBuffer, objStarter.model.indexBuffer, 0,
				VK_INDEX_TYPE_UINT32);
			std::cout << "\nkCmdBindIndexBuffer\n\n";

			// property .pipelineLayout of a pipeline contains its layout.
			// property .descriptorSets of a descriptor set contains its elements.
			vkCmdBindDescriptorSets(commandBuffer,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				P1.pipelineLayout, 0, 1, &objStarter.descriptorSet.descriptorSets[currentImage],
				0, nullptr);
			std::cout << "\nkCmdBindDescriptorSets\n\n";

			// property .indices.size() of models, contains the number of triangles * 3 of the mesh.
			vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(objStarter.model.indices.size()), 1, 0, 0, 0);
			std::cout << "\nkCmdDrawIndexed\n\n";

		}
		
	}

	// Here is where you update the uniforms.
	// Very likely this will be where you will be writing the logic of your application.
	void updateUniformBuffer(uint32_t currentImage) {

		std::cout << "\nupdateUniformBuffer\n\n";
		for (objectInitializer objStarter : listOfObjects) {
			static auto startTime = std::chrono::high_resolution_clock::now();
			auto currentTime = std::chrono::high_resolution_clock::now();
			float time = std::chrono::duration<float, std::chrono::seconds::period>
				(currentTime - startTime).count();


			UniformBufferObject ubo{};
			ubo.model = glm::rotate(glm::mat4(1.0f),
				time / 20 * glm::radians(90.0f),
				glm::vec3(0.0f, 1.0f, 0.0f));
			ubo.view = glm::lookAt(glm::vec3(0.0f, 1.0f, -5.0f),
				glm::vec3(0.0f, 0.0f, 0.0f),
				glm::vec3(0.0f, 1.0f, 0.0f));
			ubo.proj = glm::perspective(glm::radians(40.0f),
				swapChainExtent.width / (float)swapChainExtent.height,
				0.1f, 10.0f);
			ubo.proj[1][1] *= -1;

			void* data;
			vkMapMemory(device, objStarter.descriptorSet.uniformBuffersMemory[0][currentImage], 0,
				sizeof(ubo), 0, &data);
			memcpy(data, &ubo, sizeof(ubo));
			vkUnmapMemory(device, objStarter.descriptorSet.uniformBuffersMemory[0][currentImage]);
		}
		// Here is where you actually update your uniforms
		
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