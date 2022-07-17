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
#include "../graphics/engine/DirectionalLight.hpp"
#include "../graphics/engine/PointLight.hpp"
#include "../graphics/engine/SpotLight.hpp"
#include "../graphics/engine/AmbientLight.hpp"
#include "../graphics/engine/HemisphericalLight.hpp"
#include "../graphics/engine/SphericalLight.hpp"


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
using graphics::DirectionalLight;
using graphics::PointLight;
using graphics::SpotLight;
using graphics::AmbientLight;
using graphics::HemisphericalLight;
using graphics::SphericalLight;

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
		this->lastLightChangeTime = std::chrono::high_resolution_clock::now();
		//////////////////////////// SCENE 2 ///////////////////////////////////////
		GameObject* main_bird = new GameObject("main_bird", vector<Tag>{}, "objects/chuck.obj", "textures/bird.png");
		GameObject* floor = new GameObject("floor", vector<Tag>{Tag::GROUND}, "objects/floor.obj", "textures/grassPlane.jpg");
		GameObject* pig = new GameObject("pig", vector<Tag>{Tag::MOVABLE_COLLIDABLE_OBJECT}, "objects/pig.obj", "textures/bird.png");
		GameObject* pig2 = new GameObject("pig-2", vector<Tag>{Tag::MOVABLE_COLLIDABLE_OBJECT}, "objects/pig.obj", "textures/bird.png");
		GameObject* apple = new GameObject("apple-nature", vector<Tag>{Tag::ENEMY_OBJ}, "objects/apple.obj", "textures/apple.png");
		GameObject* main_arrow = new GameObject("arrow", vector<Tag>{}, "objects/arrow.obj", "textures/arrow.png");
		physicsEngine->scaleObjectInPlace(main_arrow, vec3(0.5));
		physicsEngine->scaleObjectInPlace(main_bird, vec3(3));
		physicsEngine->rotateObjectInPlace(main_arrow, vec3(0, 90, 180));
		physicsEngine->translateObjectInPlace(main_arrow, vec3(0, 0, -11.5));
		physicsEngine->translateObjectInPlace(floor, vec3(0, -7, 0));
		physicsEngine->translateObjectInPlace(apple, vec3(0, 3, -20));
		physicsEngine->rotateObjectInPlace(main_bird, vec3(0, 180, 0));
		main_arrow->setInitialTransform(main_arrow->getCurrentTransform());
		main_bird->setInitialTransform(main_bird->getCurrentTransform());
		// TODO: new allocates to heap, manage the free operation
		SphereCollider* birdCollider = new SphereCollider(1.0f);
		main_bird->setCollider(birdCollider);
		birdCollider->setGameObject(main_bird);


		vector<GameObject*> walls;
		for (int i = 0; i < 1; i++) {
			for (int j = 0; j < 2; j++) {
				GameObject* wall1 = new GameObject("wall" + to_string(i * 2 + j), vector<Tag>{Tag::RIGID_COLLIDABLE_OBJECT}, "objects/wallBox.obj", "textures/wallBox.jpg");
				physicsEngine->translateObjectInPlace(wall1, vec3(-8 + j * 2, 2 * i, -20));
				BoxCollider3D* wallCollider1 = new BoxCollider3D(wall1, 2.0f);
				wall1->setCollider(wallCollider1);
				wallCollider1->setGameObject(wall1);
				walls.push_back(wall1);
			}
		}

		for (int i = 0; i < 1; i++) {
			for (int j = 0; j < 2; j++) {
				GameObject* wall1 = new GameObject("wall" + to_string(i * 2 + j + 6), vector<Tag>{Tag::RIGID_COLLIDABLE_OBJECT}, "objects/wallBox.obj", "textures/wallBox.jpg");
				physicsEngine->translateObjectInPlace(wall1, vec3(6 + j * 2, 2 * i, -20));
				BoxCollider3D* wallCollider1 = new BoxCollider3D(wall1, 2.0f);
				wall1->setCollider(wallCollider1);
				wallCollider1->setGameObject(wall1);
				walls.push_back(wall1);
			}
		}
		physicsEngine->scaleObjectInPlace(pig, vec3(3));
		physicsEngine->translateObjectInPlace(pig, vec3(7, 2, -20));
		physicsEngine->scaleObjectInPlace(pig2, vec3(3));
		physicsEngine->translateObjectInPlace(pig2, vec3(-7, 2, -20));
		SphereCollider* pigCollider = new SphereCollider(1.0f);
		pig->setCollider(pigCollider);
		pigCollider->setGameObject(pig);

		SphereCollider* pigCollider2 = new SphereCollider(1.0f);
		pig2->setCollider(pigCollider2);
		pigCollider2->setGameObject(pig2);

		SphereCollider* appleCollider = new SphereCollider(3.0f);
		apple->setCollider(appleCollider);
		appleCollider->setGameObject(apple);


		PlaneCollider3D* floorCollider = new PlaneCollider3D(floor, 20000.f, 20000.f);
		floor->setCollider(floorCollider);
		floorCollider->setGameObject(floor);

		BoxCollider3D* arrowCollider = new BoxCollider3D(main_arrow, 1.0f);
		main_arrow->setCollider(arrowCollider);
		arrowCollider->setGameObject(main_arrow);

		Background* background = new Background("objects/MountainSkyBox.obj", "textures/MountainSkyBox.png");

		Camera* camera = new Camera();
		camera->lookAtGameObject(main_bird, vec3(50, 0, 0), vec3(0, 1, 0));
		camera->setEyeObjDir(main_bird->getCurrentPos() - camera->getCurrentPos());

		Camera* camera2 = new Camera();
		camera2->lookAtGameObject(apple, vec3(0, 5, 50), vec3(0, 1, 0));
		camera2->setEyeObjDir(apple->getCurrentPos() - camera2->getCurrentPos());

		vector<GameObject*> gameObjects = { pig, pig2, floor, main_arrow, main_bird, apple };
		for (GameObject* go : walls) {
			gameObjects.push_back(go);
		}
		vector<Camera*> cameras = { camera, camera2 };
		Scene* scene = new Scene(gameObjects, 1, cameras, background, "Nature", 0);
		scene->setDirectionalLight(new DirectionalLight(vec3(1.0, 4.0, -10.0), vec3(1.0, 1.0, 1.0)));
		scene->setAmbientLight(new AmbientLight(vec3(0.4, 0.3, 0.3), vec3(0.3, 0.3, 0.3)));
		scene->setPointLight(new PointLight(vec3(-2.f, 4.f, 8.f), vec3(1.f, 0.1f, 0.1f), 1.5f));
		scene->setSpotLight(new SpotLight(vec3(5.f, 2.f, -3.f), vec3(-2.f, 3.5f, 1.f), vec3(0.2f, 0.4f, 0.5f), 15.0f, 0.8f, 15.0f, 5.0f));

		//scene->setSpotLight(new SpotLight(vec3(1.f, 2.f, 1.f), vec3(1.f, 2.f+0.5f, 1.f ), vec3(5.0f, 5.0f, 5.0f), 1.0f, 0.8f, 15.0f, 5.0f));
		scene->setSpecularModel(graphics::SpecularModel::BLINN);
		scene->setDiffuseModel(graphics::DiffuseModel::LAMBERT);
		this->bird = main_bird;
		this->arrow = main_arrow;


		/////////////////////// SCENE 1 //////////////////////////////
		Scene* scene_city;
		GameObject* bird_2 = new GameObject("main_bird_city", vector<Tag>{}, "objects/bird.obj", "textures/bird.png");
		SphereCollider* birdCollider_2 = new SphereCollider(1.0f);
		bird_2->setCollider(birdCollider_2);
		birdCollider_2->setGameObject(bird_2);
		GameObject* main_arrow_2 = new GameObject("arrow_city", vector<Tag>{}, "objects/arrow.obj", "textures/arrow.png");
		physicsEngine->scaleObjectInPlace(main_arrow_2, vec3(0.5));
		physicsEngine->rotateObjectInPlace(main_arrow_2, vec3(0, 90, 180));
		physicsEngine->translateObjectInPlace(main_arrow_2, vec3(0, 0, -11.5));
		main_arrow_2->setInitialTransform(main_arrow_2->getCurrentTransform());

		physicsEngine->scaleObjectInPlace(bird_2, vec3(3));
		physicsEngine->rotateObjectInPlace(bird_2, vec3(0, 180, 0));
		bird_2->setInitialTransform(bird_2->getCurrentTransform());
		BoxCollider3D* arrowCollider_2 = new BoxCollider3D(main_arrow_2, 1.0f);
		main_arrow_2->setCollider(arrowCollider_2);
		arrowCollider_2->setGameObject(main_arrow_2);

		GameObject* apple_2 = new GameObject("apple", vector<Tag>{Tag::ENEMY_OBJ}, "objects/apple.obj", "textures/apple.png");
		SphereCollider* appleCollider_2 = new SphereCollider(3.5f);
		apple_2->setCollider(appleCollider_2);
		appleCollider_2->setGameObject(apple_2);

		physicsEngine->scaleObjectInPlace(apple_2, vec3(2.f));
		physicsEngine->translateObjectInPlace(apple_2, vec3(0.f, 5.f, -30.f));
		vector<GameObject*> walls_2;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 5; j++) {
				GameObject* wall1 = new GameObject("wall_2_" + to_string(i * 5 + j), vector<Tag>{Tag::RIGID_COLLIDABLE_OBJECT}, "objects/wallBox.obj", "textures/wallBox.jpg");
				physicsEngine->rotateObjectInPlace(wall1, vec3(90, 0, 90));
				physicsEngine->translateObjectInPlace(wall1, vec3(-4 + j * 2, -4.5 + 2 * i, -20));
				BoxCollider3D* wallCollider1 = new BoxCollider3D(wall1, 2.0f);
				wall1->setCollider(wallCollider1);
				wallCollider1->setGameObject(wall1);
				walls_2.push_back(wall1);
			}
		}
		Camera* camera_2 = new Camera();
		camera_2->lookAtGameObject(bird_2, vec3(50, 0, 0), vec3(0, 1, 0));
		camera_2->setEyeObjDir(bird_2->getCurrentPos() - camera_2->getCurrentPos());

		Camera* camera2_2 = new Camera();
		camera2_2->lookAtGameObject(apple_2, vec3(0, 10, 50), vec3(0, 1, 0));
		camera2_2->setEyeObjDir(apple_2->getCurrentPos() - camera_2->getCurrentPos());
		vector<Camera*> cameras_2 = { camera_2, camera2_2 };

		Background* background_2 = new Background("objects/MountainSkyBox.obj", "textures/0-city-skybox.png");

		GameObject* floor_2 = new GameObject("floor", vector<Tag>{Tag::GROUND}, "objects/floor.obj", "textures/city-ground.png");
		physicsEngine->translateObjectInPlace(floor_2, vec3(0.f, -7.f, 0.f));
		PlaneCollider3D* floorCollider_2 = new PlaneCollider3D(floor_2, 20000.f, 20000.f);
		floor_2->setCollider(floorCollider_2);
		floorCollider_2->setGameObject(floor_2);

		vector<GameObject*> gameObjects_2 = { bird_2, floor_2, main_arrow_2, apple_2 };
		for (GameObject* go : walls_2) {
			gameObjects_2.push_back(go);
		}
		scene_city = new Scene(gameObjects_2, 1, cameras_2, background_2, "City", 1);
		scene_city->setDirectionalLight(new DirectionalLight(vec3(0.5, 2.5, 0.5), vec3(0.5, 0.3, 0.7)));
		scene_city->setAmbientLight(new AmbientLight(vec3(1, 1, 1), vec3(0.3, 0, 0)));
		scene_city->setPointLight(new PointLight(vec3(10.f, 6.f, -8.f), vec3(0.f, 0.6f, 0.4f), 1.5f));
		scene_city->setSpotLight(new SpotLight(vec3(-5.f, 4.f, 3.f), vec3(2.f, 0.f, 0.f), vec3(0.5f, 0.4f, 0.1f), 15.0f, 0.8f, 15.0f, 5.0f));

		this->bird = bird_2;
		this->arrow = main_arrow_2;
		scene_city->setSpecularModel(graphics::SpecularModel::BLINN);
		scene_city->setDiffuseModel(graphics::DiffuseModel::LAMBERT);

		/////////////////////// Scene 3 /////////////////////////////////////


		GameObject* bird_3 = new GameObject("main_bird_sand", vector<Tag>{}, "objects/bubbles.obj", "textures/bird.png");
		SphereCollider* birdCollider_3 = new SphereCollider(1.0f);
		bird_3->setCollider(birdCollider_3);
		birdCollider_3->setGameObject(bird_3);
		GameObject* main_arrow_3 = new GameObject("arrow_sand", vector<Tag>{}, "objects/arrow.obj", "textures/arrow.png");
		physicsEngine->scaleObjectInPlace(main_arrow_3, vec3(0.5));
		physicsEngine->rotateObjectInPlace(main_arrow_3, vec3(0, 90, 180));
		physicsEngine->translateObjectInPlace(main_arrow_3, vec3(0, 0, -11.5));
		main_arrow_3->setInitialTransform(main_arrow_3->getCurrentTransform());

		physicsEngine->scaleObjectInPlace(bird_3, vec3(5));
		physicsEngine->rotateObjectInPlace(bird_3, vec3(0, 180, 0));
		bird_3->setInitialTransform(bird_3->getCurrentTransform());
		BoxCollider3D* arrowCollider_3 = new BoxCollider3D(main_arrow_3, 1.0f);
		main_arrow_3->setCollider(arrowCollider_3);
		arrowCollider_3->setGameObject(main_arrow_3);

		GameObject* apple_3 = new GameObject("apple_sand", vector<Tag>{Tag::ENEMY_OBJ}, "objects/apple.obj", "textures/apple.png");
		SphereCollider* appleCollider_3 = new SphereCollider(4.5f);
		apple_3->setCollider(appleCollider_3);
		appleCollider_3->setGameObject(apple_3);

		GameObject* pig_3 = new GameObject("pig_sand", vector<Tag>{Tag::RIGID_COLLIDABLE_OBJECT}, "objects/pigking.obj", "textures/bird.png");
		SphereCollider* pigCollider_3 = new SphereCollider(4.f);
		pig_3->setCollider(pigCollider_3);
		pigCollider_3->setGameObject(pig_3);

		physicsEngine->scaleObjectInPlace(pig_3, vec3(7.f));
		physicsEngine->translateObjectInPlace(pig_3, vec3(0.f, 6.f, -28.f));

		physicsEngine->scaleObjectInPlace(apple_3, vec3(2.f));
		physicsEngine->translateObjectInPlace(apple_3, vec3(0.f, 4.5f, -62.f));

		Camera* camera_3 = new Camera();
		camera_3->lookAtGameObject(bird_3, vec3(50, 0, 0), vec3(0, 1, 0));
		camera_3->setEyeObjDir(bird_3->getCurrentPos() - camera_3->getCurrentPos());

		Camera* camera3_2 = new Camera();
		camera3_2->lookAtGameObject(apple_3, vec3(0, 10, 50), vec3(0, 1, 0));
		camera3_2->setEyeObjDir(apple_3->getCurrentPos() - camera3_2->getCurrentPos());
		vector<Camera*> cameras_3 = { camera_3, camera3_2 };

		Background* background_3 = new Background("objects/MountainSkyBox.obj", "textures/Skybox.png");

		GameObject* floor_3 = new GameObject("floor", vector<Tag>{Tag::GROUND}, "objects/floor.obj", "textures/sand_ground.jpg");
		physicsEngine->translateObjectInPlace(floor_3, vec3(0.f, -7.f, 0.f));
		PlaneCollider3D* floorCollider_3 = new PlaneCollider3D(floor_3, 20000.f, 20000.f);
		floor_3->setCollider(floorCollider_3);
		floorCollider_3->setGameObject(floor_3);

		vector<GameObject*> walls_3;
		for (int i = 0; i < 6; i++) { // number of stairs
			for (int j = 0; j < 5; j++) {
				GameObject* wall1 = new GameObject("wall_3_" + to_string(i * 5 + j), vector<Tag>{Tag::RIGID_COLLIDABLE_OBJECT}, "objects/wallBox.obj", "textures/wallBox.jpg");
				physicsEngine->rotateObjectInPlace(wall1, vec3(90, 45, 90));
				physicsEngine->translateObjectInPlace(wall1, vec3(-4 + j * 2, -3 + i, -(13 + 2 * i)));
				BoxCollider3D* wallCollider1 = new BoxCollider3D(wall1, 2.0f);
				wall1->setCollider(wallCollider1);
				wallCollider1->setGameObject(wall1);
				walls_3.push_back(wall1);
			}
		}

		for (int i = 0; i < 6; i++) { // number of stairs
			for (int j = 0; j < 5; j++) {
				GameObject* wall1 = new GameObject("wall_31_" + to_string(i * 5 + j), vector<Tag>{Tag::RIGID_COLLIDABLE_OBJECT}, "objects/wallBox.obj", "textures/wallBox.jpg");
				physicsEngine->rotateObjectInPlace(wall1, vec3(90, 45, 90));
				physicsEngine->translateObjectInPlace(wall1, vec3(-4 + j * 2, 18 - i, -(13 + 2 * i)));
				BoxCollider3D* wallCollider1 = new BoxCollider3D(wall1, 2.0f);
				wall1->setCollider(wallCollider1);
				wallCollider1->setGameObject(wall1);
				walls_3.push_back(wall1);
			}
		}

		vector<GameObject*> gameObjects_3 = { bird_3, floor_3, main_arrow_3, apple_3, pig_3 };
		for (GameObject* go : walls_3) {
			gameObjects_3.push_back(go);
		}
		Scene* scene_beach = new Scene(gameObjects_3, 0, cameras_3, background_3, "Sand", 2);
		scene_beach->setDirectionalLight(new DirectionalLight(vec3(1, 1, 1), vec3(0.2, 0.2, 0.2)));
		scene_beach->setAmbientLight(new AmbientLight(vec3(1, 1, 1), vec3(0.3, 0, 0)));
		scene_beach->setSpotLight(new SpotLight(glm::vec3(0.f, 0.f, 0.f), glm::vec3(10.f, 25.f, 10.f), glm::vec3(1.f, 1.f, 1.f), 30.f));
		scene_beach->setPointLight(new PointLight(vec3(10.f, 6.f, -8.f), vec3(0.f, 0.6f, 0.4f), 1.5f));
		scene_beach->setSpecularModel(graphics::SpecularModel::TOON);
		scene_beach->setDiffuseModel(graphics::DiffuseModel::TOON);
		/////////////////////// Final Call to Graphics Engine //////////////////////////

		graphicsEngine->addScenes({ scene, scene_city, scene_beach });
		graphicsEngine->selectScene("City");
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
		if (glfwGetKey(this->graphicsEngine->window, GLFW_KEY_1))
		{
			Scene* futureScene = graphicsEngine->getScene("City");
			this->arrow = futureScene->getGameObjectPointerByName("arrow_city");
			this->bird = futureScene->getGameObjectPointerByName("main_bird_city");
			this->graphicsEngine->selectScene("City");
			resetCoordinates();
		}
		if (glfwGetKey(this->graphicsEngine->window, GLFW_KEY_2))
		{
			Scene* futureScene = graphicsEngine->getScene("Nature");
			this->arrow = futureScene->getGameObjectPointerByName("arrow");
			this->bird = futureScene->getGameObjectPointerByName("main_bird");
			this->graphicsEngine->selectScene("Nature");
			resetCoordinates();
		}
		if (glfwGetKey(this->graphicsEngine->window, GLFW_KEY_3))
		{
			Scene* futureScene = graphicsEngine->getScene("Sand");
			this->arrow = futureScene->getGameObjectPointerByName("arrow_sand");
			this->bird = futureScene->getGameObjectPointerByName("main_bird_sand");
			this->graphicsEngine->selectScene("Sand");
			resetCoordinates();
		}
		if (glfwGetKey(this->graphicsEngine->window, GLFW_KEY_P))
		{   
			DirectionalLight* dlPointer = this->graphicsEngine->activeScene->getDirectionalLightPointer();
			if (dlPointer != NULL) {
				dlPointer->setLightDir(dlPointer->getLightDir() + vec3(0.001, 0, 0));
			}
		}
		if (glfwGetKey(this->graphicsEngine->window, GLFW_KEY_O))
		{
			DirectionalLight* dlPointer = this->graphicsEngine->activeScene->getDirectionalLightPointer();
			if (dlPointer != NULL) {
				dlPointer->setLightDir(dlPointer->getLightDir() + vec3(-0.001, 0, 0));
			}
		}
		if (glfwGetKey(this->graphicsEngine->window, GLFW_KEY_E)) {
			this->physicsEngine->rotateObjectInPlace(camera, vec3(0, 0.2f, 0));
		}
		if (glfwGetKey(this->graphicsEngine->window, GLFW_KEY_Q)) {
			this->physicsEngine->rotateObjectInPlace(camera, vec3(0, -0.2f, 0));
		}
		if (glfwGetKey(this->graphicsEngine->window, GLFW_KEY_UP)) {
			this->physicsEngine->translateObjectInPlace(camera, deltaT * vec3(0,0,1));
		}
		if (glfwGetKey(this->graphicsEngine->window, GLFW_KEY_DOWN)) {
			this->physicsEngine->translateObjectInPlace(camera, deltaT * vec3(0, 0, -1));
		}
		if (glfwGetKey(this->graphicsEngine->window, GLFW_KEY_LEFT)) {
			this->physicsEngine->translateObjectInPlace(camera, deltaT * vec3(1, 0, 0));
		}
		if (glfwGetKey(this->graphicsEngine->window, GLFW_KEY_RIGHT)) {
			this->physicsEngine->translateObjectInPlace(camera, deltaT * vec3(-1, 0, 0));
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

		if (glfwGetKey(this->graphicsEngine->window, GLFW_KEY_F5)) {
			this->graphicsEngine->activeScene->setDiffuseModel(graphics::DiffuseModel::LAMBERT);
		}
		if (glfwGetKey(this->graphicsEngine->window, GLFW_KEY_F6)) {
			this->graphicsEngine->activeScene->setDiffuseModel(graphics::DiffuseModel::TOON);
		}
		if (glfwGetKey(this->graphicsEngine->window, GLFW_KEY_F7)) {
			this->graphicsEngine->activeScene->setDiffuseModel(graphics::DiffuseModel::OREN_NAYAR);
		}
		if (glfwGetKey(this->graphicsEngine->window, GLFW_KEY_F8)) {
			this->graphicsEngine->activeScene->setSpecularModel(graphics::SpecularModel::BLINN);
		}
		if (glfwGetKey(this->graphicsEngine->window, GLFW_KEY_F9)) {
			this->graphicsEngine->activeScene->setSpecularModel(graphics::SpecularModel::PHONG);
		}
		if (glfwGetKey(this->graphicsEngine->window, GLFW_KEY_F10)) {
			this->graphicsEngine->activeScene->setSpecularModel(graphics::SpecularModel::TOON);
		}

		if (glfwGetKey(this->graphicsEngine->window, GLFW_KEY_F1)) {
			auto currentTime = std::chrono::high_resolution_clock::now();
			float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>
				(currentTime - this->lastLightChangeTime).count();
			if (deltaTime > 0.5) {
				this->graphicsEngine->activeScene->useDirectionalLight();
				this->lastLightChangeTime = std::chrono::high_resolution_clock::now();
			}
			
		}
		if (glfwGetKey(this->graphicsEngine->window, GLFW_KEY_F2)) {
			auto currentTime = std::chrono::high_resolution_clock::now();
			float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>
				(currentTime - this->lastLightChangeTime).count();
			if (deltaTime > 0.5) {
				this->graphicsEngine->activeScene->usePointLight();
				this->lastLightChangeTime = std::chrono::high_resolution_clock::now();
			}
		}
		if (glfwGetKey(this->graphicsEngine->window, GLFW_KEY_F3)) {
			auto currentTime = std::chrono::high_resolution_clock::now();
			float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>
				(currentTime - this->lastLightChangeTime).count();
			if (deltaTime > 0.5) {
				this->graphicsEngine->activeScene->useSpotLight();
				this->lastLightChangeTime = std::chrono::high_resolution_clock::now();
			}
		}
		if (glfwGetKey(this->graphicsEngine->window, GLFW_KEY_F4)) {
			auto currentTime = std::chrono::high_resolution_clock::now();
			float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>
				(currentTime - this->lastLightChangeTime).count();
			if (deltaTime > 0.5) {
				this->graphicsEngine->activeScene->useAmbientLight();
				this->lastLightChangeTime = std::chrono::high_resolution_clock::now();
			}
		}



		if (glfwGetKey(this->graphicsEngine->window, GLFW_KEY_R)) {
			resetCoordinates();
		}
		if (glfwGetKey(this->graphicsEngine->window, GLFW_KEY_L) && !launched) {
			float module = 40.f;
			float radAngle1 = radians(this->arrowAngle1);
			float radAngle2 = radians(this->arrowAngle2);
			vec3 launchDir = vec3(-module * cos(radAngle1) * sin(radAngle2), module * sin(radAngle1), -module * cos(radAngle1) * cos(radAngle2));
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

	void Application::handleAutomaticMovement(float deltaTime, int iteration) {
		float speed = 2.5;
		float rotSpeed = 90;
		static int debounce;
		if (iteration <= 1) {
			debounce = iteration;
		}
		float deltaT = deltaTime * speed;
		float rotT = deltaTime * rotSpeed;
		Scene* currentScene = graphicsEngine->getCurrentScene();
		static auto sign = -1;
		if (currentScene->getName() == "Sand") {
			GameObject* pig = currentScene->getGameObjectPointerByName("pig_sand");
			// To implement this as a per-object field && put debounce
			// In this case debounce is the number of frames instead of time
			if (pig->getCurrentPos().y > 14.5f && iteration - debounce > 500) {
				sign = -sign;
				debounce = iteration;
			}
			if (pig->getCurrentPos().y < 1.5f && iteration - debounce > 500) {
				sign = -sign;
				debounce = iteration;
			}
			physicsEngine->translateObjectInPlace(pig, sign * deltaT * glm::vec3(0.f, 1.f, 0.f));
			//physicsEngine->rotateObject(pig, rotT * glm::vec3( 0, 1, 0));
		}

	}

	void Application::resetCoordinates() {
		launched = false;
		this->arrowAngle1 = 0;
		this->arrowAngle2 = 0;
		bird->setCurrentTransform(bird->getInitialTransform());
		bird->setVelocity(vec3(0, 0, 0));
		bird->getCollider()->setLastCollision("None");
	}
	void Application::update(float deltaTime)
	{
		handleCommands(deltaTime);
		static int iteration = 1;
		handleAutomaticMovement(deltaTime, iteration); 
		iteration += 1;
	}
}