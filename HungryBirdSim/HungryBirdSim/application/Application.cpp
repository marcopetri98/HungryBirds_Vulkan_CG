#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>

#include "Application.h"
#include "../graphics/engine/DummyRecursionSolver.hpp"
#include "../graphics/engine/Scene.hpp"
#include "../graphics/engine/GameObject.hpp"
#include "../graphics/engine/Camera.hpp"
#include "../graphics/engine/Background.hpp"

using graphics::Scene;
using graphics::GameObject;
using graphics::Camera;
using graphics::Background;
using std::vector;

namespace app
{
	Application::Application(string title, int width, int height)
	{
		graphicsEngine = GraphicsEngine(title, width, height);

		GameObject bird = GameObject("bird", 0, "objects/bird.obj", "textures/bird.png");
		Camera camera = Camera();
		camera.lookAtGameObject(bird, vec3(10, 0, 10), vec3(0, 1, 0));
		vector<GameObject> gameObjects = { bird };
		vector<Camera> cameras = { camera };
		Scene scene = Scene(gameObjects, 0, cameras, Background(), "try", 0);

		graphicsEngine.addScenes({ scene });
		graphicsEngine.selectScene("try");
	}
	
	int Application::start()
	{
		try
		{
			graphicsEngine.run();
		}
		catch (const std::exception& e)
		{
			std::cerr << e.what() << std::endl;
			return EXIT_FAILURE;
		}

		return EXIT_SUCCESS;
	}
}