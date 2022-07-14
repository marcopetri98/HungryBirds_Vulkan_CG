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

		GameObject* bird = new GameObject("bird", vector<Tag>{}, "objects/bird.obj", "textures/bird.png");
		GameObject* bird2 = new GameObject("bird2", vector<Tag>{Tag::RIGID_COLLIDABLE_OBJECT}, "objects/bird.obj", "textures/bird.png");
		GameObject* bird3 = new GameObject("bird3", vector<Tag>{Tag::GROUND}, "objects/bird.obj", "textures/bird.png");
		GameObject* bird4 = new GameObject("bird4", vector<Tag>{Tag::GROUND}, "objects/bird.obj", "textures/bird.png");
		GameObject* bird5 = new GameObject("bird5", vector<Tag>{Tag::GROUND}, "objects/bird.obj", "textures/bird.png");


		physicsEngine->translateObjectInPlace(bird2, vec3(0, 5, 10));
		physicsEngine->translateObjectInPlace(bird3, vec3(0, -5, 3));
		physicsEngine->translateObjectInPlace(bird4, vec3(0, -10, 10));
		physicsEngine->translateObjectInPlace(bird5, vec3(0, 10, 10));
		bird->setVelocity(vec3(0, 5.f, 10.f));

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

		Camera* camera = new Camera();
		camera->lookAtGameObject(bird, vec3(100, 10, 0), vec3(0, 1, 0));
		vector<GameObject*> gameObjects = { bird, bird2, bird3, bird4, bird5 };
		vector<Camera*> cameras = { camera };
		Scene* scene = new Scene(gameObjects, 0, cameras, Background(), "try", 0);

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
	}

	void Application::update(float deltaTime)
	{
		handleCommands(deltaTime);
		// TODO: implement application game logic
	}
}