#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>

#include "Application.h"
#include "../physics/PhysicsEngine.hpp"
#include "../graphics/engine/DummyRecursionSolver.hpp"
#include "../graphics/engine/Scene.hpp"
#include "../graphics/engine/GameObject.hpp"
#include "../graphics/engine/Camera.hpp"
#include "../graphics/engine/Background.hpp"
#include "../graphics/engine/Tags.hpp"
#include "../graphics/engine/SphereCollider.hpp"

using graphics::Scene;
using graphics::GameObject;
using graphics::Camera;
using graphics::Background;
using graphics::SphereCollider;
using std::vector;
using tags::Tag;
using physics::PhysicsEngine;


namespace app
{
	Application::Application(string title, int width, int height)
	{
		graphicsEngine = new GraphicsEngine(title, width, height);
		physicsEngine = new PhysicsEngine(true);
		graphicsEngine->setPhysicsEngine(physicsEngine);
		graphicsEngine->setApplication(this);

		GameObject* bird = new GameObject("bird", vector<Tag>{}, "objects/bird.obj", "textures/bird.png");
		GameObject* bird2 = new GameObject("bird2", vector<Tag>{Tag::RIGID_COLLIDABLE_OBJECT}, "objects/bird.obj", "textures/bird.png");
		GameObject* bird3 = new GameObject("bird3", vector<Tag>{Tag::GROUND}, "objects/bird.obj", "textures/bird.png");
		GameObject* bird4 = new GameObject("bird4", vector<Tag>{Tag::GROUND}, "objects/bird.obj", "textures/bird.png");
		GameObject* bird5 = new GameObject("bird5", vector<Tag>{Tag::GROUND}, "objects/bird.obj", "textures/bird.png");


		physicsEngine->translateObjectInPlace(bird2, vec3(0, 5, 10));
		physicsEngine->translateObjectInPlace(bird3, vec3(0, -5, 3));
		physicsEngine->translateObjectInPlace(bird4, vec3(0, -10, 10));
		physicsEngine->translateObjectInPlace(bird5, vec3(0, 10, 10));
		//bird->setVelocity(vec3(0, 9.f, 10.f));

		// TODO: new allocates to heap, manage the free operation
		SphereCollider* birdCollider = new SphereCollider(1.0f);
		bird->setCollider(birdCollider);
		birdCollider->setGameObject(bird);

		SphereCollider* birdCollider2 = new SphereCollider(1.0f);
		bird2->setCollider(birdCollider2);
		birdCollider2->setGameObject(bird2);

		SphereCollider* birdCollider3 = new SphereCollider(1.0f);
		bird3->setCollider(birdCollider3);
		birdCollider3->setGameObject(bird3);

		SphereCollider* birdCollider4 = new SphereCollider(1.0f);
		bird4->setCollider(birdCollider4);
		birdCollider4->setGameObject(bird4);

		SphereCollider* birdCollider5 = new SphereCollider(1.0f);
		bird5->setCollider(birdCollider5);
		birdCollider5->setGameObject(bird5);

		Background* background = new Background("objects/MountainSkyBox.obj", "textures/MountainSkyBox.png");

		Camera* camera = new Camera();
		camera->lookAtGameObject(bird, vec3(100, 0, 0), vec3(0, 1, 0));
		camera->setEyeObjDir(bird->getCurrentPos() - camera->getCurrentPos());

		Camera* camera2 = new Camera();
		camera2->lookAtGameObject(bird2, vec3(0, 10, -100), vec3(0, 1, 0));
		camera2->setEyeObjDir(bird2->getCurrentPos() - camera2->getCurrentPos());

		vector<GameObject*> gameObjects = { bird, bird2, bird3, bird4, bird5 };
		vector<Camera*> cameras = { camera, camera2 };
		Scene* scene = new Scene(gameObjects, 0, cameras, background, "try", 0);

		graphicsEngine->addScenes({ scene });
		graphicsEngine->selectScene("try");
	}
	
	int Application::start()
	{
		try
		{
			graphicsEngine->run();
			// TODO: destroy graphics engine (manage scene deallocation)
		}
		catch (const std::exception& e)
		{
			std::cerr << e.what() << std::endl;
			return EXIT_FAILURE;
		}

		return EXIT_SUCCESS;
	}

	void Application::handleCommands(float deltaTime)
	{
		// TODO: handle key presses and high level commands with physics engine and eventually graphics engine API calls
		float speed = 19.89;
		float deltaT = deltaTime * speed;
		Camera* camera = this->graphicsEngine->activeScene->getCamera();
		vector<vec3> cameraDirsFRBL = camera->getMovingDirsFRBL();
		if (glfwGetKey(this->graphicsEngine->window, GLFW_KEY_UP)) {
			this->physicsEngine->translateObjectInPlace(camera, deltaT*cameraDirsFRBL[0]);
		}
		if (glfwGetKey(this->graphicsEngine->window, GLFW_KEY_DOWN)) {
			this->physicsEngine->translateObjectInPlace(camera, deltaT * cameraDirsFRBL[2]);
		}
		if (glfwGetKey(this->graphicsEngine->window, GLFW_KEY_LEFT)) {
			this->physicsEngine->translateObjectInPlace(camera, deltaT * cameraDirsFRBL[3]);
		}
		if (glfwGetKey(this->graphicsEngine->window, GLFW_KEY_RIGHT)) {
			this->physicsEngine->translateObjectInPlace(camera, deltaT * cameraDirsFRBL[1]);
		}
		if (glfwGetKey(this->graphicsEngine->window, GLFW_KEY_SPACE)) {
			this->physicsEngine->translateObjectInPlace(camera, deltaT * vec3(0, -1, 0));
		}
		if (glfwGetKey(this->graphicsEngine->window, GLFW_KEY_LEFT_SHIFT)) {
			this->physicsEngine->translateObjectInPlace(camera, deltaT * vec3(0, 1, 0));
		}
		if (glfwGetKey(this->graphicsEngine->window, GLFW_KEY_C)) {
			//TODO parametrize this
			this->graphicsEngine->activeScene->setCamera(1);
		}
	}

	void Application::update(float deltaTime)
	{
		handleCommands(deltaTime);
		// TODO: implement application game logic
	}
}