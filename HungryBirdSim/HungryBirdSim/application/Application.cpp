#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <chrono>

#include "Application.h"
#include "../RecSolver.h"
#include "glm/glm.hpp"
#include "../physics/PhysicsEngine.hpp"
#include "../graphics/engine/DummyRecursionSolver.hpp"
#include "../graphics/engine/Scene.hpp"
#include "../graphics/engine/GameObject.hpp"
#include "../graphics/engine/Camera.hpp"
#include "../graphics/engine/Background.hpp"
#include "../graphics/engine/Tags.hpp"
#include "../graphics/engine/SphereCollider.hpp"
#include "../graphics/engine/BoxCollider3D.hpp"
#include "../graphics/engine/PlaneCollider3d.hpp"


using Clock = std::chrono::high_resolution_clock;
using graphics::Scene;
using graphics::GameObject;
using graphics::Camera;
using graphics::Background;
using graphics::SphereCollider, graphics::BoxCollider3D, graphics::PlaneCollider3D;
using std::vector, std::to_string;
using tags::Tag;
using physics::PhysicsEngine;
using glm::radians;


namespace app
{
	Application::Application(string title, int width, int height)
	{
		graphicsEngine = new GraphicsEngine(title, width, height, 0.01f, 10000.0f);
		physicsEngine = new PhysicsEngine(true);
		graphicsEngine->setPhysicsEngine(physicsEngine);
		graphicsEngine->setApplication(this);
		this->arrowAngle1 = 0;
		this->arrowAngle2 = 0;
		this->launched = false;
		this->lastCameraChangeTime = std::chrono::high_resolution_clock::now();

		this->bird = new GameObject("bird", vector<Tag>{}, "objects/bird.obj", "textures/bird.png");
		GameObject* bird2 = new GameObject("bird2", vector<Tag>{Tag::RIGID_COLLIDABLE_OBJECT}, "objects/bird.obj", "textures/bird.png");
		GameObject* bird4 = new GameObject("bird4", vector<Tag>{Tag::RIGID_COLLIDABLE_OBJECT}, "objects/bird.obj", "textures/bird.png");
		GameObject* bird5 = new GameObject("bird5", vector<Tag>{Tag::RIGID_COLLIDABLE_OBJECT}, "objects/bird.obj", "textures/bird.png");
		GameObject* floor = new GameObject("floor", vector<Tag>{Tag::GROUND}, "objects/floor.obj", "textures/floor.png");
		this->arrow = new GameObject("arrow", vector<Tag>{}, "objects/arrow.obj", "textures/arrow.png");

		physicsEngine->translateObjectInPlace(bird2, vec3(0, 5, -10));
		physicsEngine->translateObjectInPlace(bird4, vec3(0, -10, 10));
		physicsEngine->translateObjectInPlace(bird5, vec3(0, 10, 10));
		physicsEngine->scaleObjectInPlace(arrow, vec3(0.5));
		physicsEngine->rotateObjectInPlace(arrow, vec3(0, 90, 0));
		physicsEngine->translateObjectInPlace(arrow, vec3(0, 0, -11.5));
		physicsEngine->translateObjectInPlace(floor, vec3(0, -20, 0));
		arrow->setInitialTransform(arrow->getCurrentTransform());
		//bird->setVelocity(vec3(0, 9.f, 10.f));

		// TODO: new allocates to heap, manage the free operation
		SphereCollider* birdCollider = new SphereCollider(1.0f);
		bird->setCollider(birdCollider);
		birdCollider->setGameObject(bird);

		SphereCollider* birdCollider2 = new SphereCollider(1.0f);
		bird2->setCollider(birdCollider2);
		birdCollider2->setGameObject(bird2);

		vector<GameObject*> walls;
		for (int i = 0; i < 1; i++) {
			for (int j = 0; j < 2; j++) {
				GameObject* wall1 = new GameObject("wall"+to_string(i*10+j), vector<Tag>{Tag::RIGID_COLLIDABLE_OBJECT}, "objects/wallBox.obj", "textures/wallBox.jpg");
				physicsEngine->translateObjectInPlace(wall1, vec3(-10 + j * 2, 3+2*i, -20));
				BoxCollider3D* wallCollider1 = new BoxCollider3D(wall1, 2.0f);
				wall1->setCollider(wallCollider1);
				wallCollider1->setGameObject(wall1);
				walls.push_back(wall1);
			}
		}

		PlaneCollider3D* floorCollider = new PlaneCollider3D(floor, 20000.f, 20000.f);
		floor->setCollider(floorCollider);
		floorCollider->setGameObject(floor);

		SphereCollider* birdCollider4 = new SphereCollider(1.0f);
		bird4->setCollider(birdCollider4);
		birdCollider4->setGameObject(bird4);

		SphereCollider* birdCollider5 = new SphereCollider(1.0f);
		bird5->setCollider(birdCollider5);
		birdCollider5->setGameObject(bird5);

		BoxCollider3D* arrowCollider = new BoxCollider3D(bird, 1.0f);
		arrow->setCollider(arrowCollider);
		arrowCollider->setGameObject(arrow);

		Background* background = new Background("objects/MountainSkyBox.obj", "textures/MountainSkyBox.png");

		Camera* camera = new Camera();
		camera->lookAtGameObject(bird, vec3(100, 0, 0), vec3(0, 1, 0));
		camera->setEyeObjDir(bird->getCurrentPos() - camera->getCurrentPos());

		Camera* camera2 = new Camera();
		camera2->lookAtGameObject(bird2, vec3(0, 10, 100), vec3(0, 1, 0));
		camera2->setEyeObjDir(bird2->getCurrentPos() - camera2->getCurrentPos());

		vector<GameObject*> gameObjects = { bird, bird2, bird4, bird5, arrow, floor };
		for (GameObject* go : walls) {
			gameObjects.push_back(go);
		}
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
		float speed = 50;
		float deltaT = deltaTime * speed;
		Camera* camera = this->graphicsEngine->activeScene->getCamera();
		vector<vec3> cameraDirsFRBL = camera->getMovingDirsFRBL();
		if (glfwGetKey(this->graphicsEngine->window, GLFW_KEY_UP)) {
			this->physicsEngine->translateObjectInPlace(camera, deltaT * cameraDirsFRBL[0]);
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
			auto currentTime = std::chrono::high_resolution_clock::now();
			float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>
				(currentTime - this->lastCameraChangeTime).count();
			if (deltaTime > 0.5) {
				Scene* currScene = this->graphicsEngine->activeScene;
				int newCameraIdx = (currScene->getCurrCameraIdx() + 1) % currScene->getAvailableCameras().size();
				currScene->setCamera(newCameraIdx);
				currScene->setCurrCameraIdx(newCameraIdx);
				this->lastCameraChangeTime = std::chrono::high_resolution_clock::now();
			}
		}
		if (glfwGetKey(this->graphicsEngine->window, GLFW_KEY_R)) {
			launched = false;
			this->arrowAngle1 = 0;
			this->arrowAngle2 = 0;
			bird->setCurrentTransform(bird->getInitialTransform());
			bird->setVelocity(vec3(0, 0, 0));
			bird->getCollider()->setLastCollision("None");
		}
		if (glfwGetKey(this->graphicsEngine->window, GLFW_KEY_L) && !launched) {
			float module = 40.f;
			float radAngle1 = radians(this->arrowAngle1);
			float radAngle2 = radians(this->arrowAngle2);
			vec3 launchDir = vec3(-module*cos(radAngle1)*sin(radAngle2), module * sin(radAngle1), -module * cos(radAngle1) * cos(radAngle2));
			this->bird->setVelocity(launchDir);
			physicsEngine->hideObject(arrow);
			this->launched = true;

		}
		if (!this->launched) {
			//Arrow
			if (glfwGetKey(this->graphicsEngine->window, GLFW_KEY_W)) {
				this->arrowAngle1 += 0.01f;
			}
			if (glfwGetKey(this->graphicsEngine->window, GLFW_KEY_A)) {
				this->arrowAngle2 += 0.01f;
			}
			if (glfwGetKey(this->graphicsEngine->window, GLFW_KEY_S)) {
				this->arrowAngle1 -= 0.01f;
			}
			if (glfwGetKey(this->graphicsEngine->window, GLFW_KEY_D)) {
				this->arrowAngle2 -= 0.01f;
			}

			if (this->arrowAngle1 >= 25) {
				this->arrowAngle1 = 25;
			}
			if (this->arrowAngle1 <= -25) {
				this->arrowAngle1 = -25;
			}
			if (this->arrowAngle2 >= 25) {
				this->arrowAngle2 = 25;
			}
			if (this->arrowAngle2 <= -25) {
				this->arrowAngle2 = -25;
			}
			arrow->setCurrentTransform(arrow->getInitialTransform());
			physicsEngine->translateObjectInPlace(arrow, vec3(0, 0, 1));
			physicsEngine->rotateObjectInPlace(arrow, vec3(this->arrowAngle1, this->arrowAngle2, 0));
			physicsEngine->translateObjectInPlace(arrow, -vec3(0, 0, 1));
		}
	}

	void Application::update(float deltaTime)
	{
		handleCommands(deltaTime);
		// TODO: implement application game logic
	}
}