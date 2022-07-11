
#include <stdexcept>

#include "Scene.hpp"
#include "../../utils/CollectionUtils.hpp"

using collectionutils::vectorContains;

namespace graphics
{
	Scene::Scene(vector<GameObject> gameObjects, int camera, vector<Camera> availableCameras, Background background, string name, int id)
	{
		this->gameObjectsNames.resize(gameObjects.size());
		this->gameObjects.resize(gameObjects.size());

		for (GameObject gameObject : gameObjects)
		{
			if (this->gameObjectsNames.size() != 0 && vectorContains(this->gameObjectsNames, gameObject.getName()))
			{
				throw std::runtime_error("Two objects have the same name, objects ids must be unique");
			}
			else
			{
				this->gameObjectsNames.push_back(gameObject.getName());
				this->gameObjects.push_back(gameObject);
				this->mapNamesGameObjects.insert(std::pair<string, GameObject>(gameObject.getName(), gameObject));
			}
		}

		this->camera = availableCameras[camera];
		this->availableCameras = availableCameras;
		this->background = background;
		this->name = name;
		this->id = id;
	}

	GameObject Scene::getGameObjectByName(string name)
	{
		if (!vectorContains(this->gameObjectsNames, name))
		{
			throw std::runtime_error("There isn't a game object with name: " + name);
		}
		else
		{
			return this->mapNamesGameObjects[name];
		}
	}

	Camera Scene::getCamera()
	{
		return this->camera;
	}

	vector<Camera> Scene::getAvailableCameras()
	{
		return this->availableCameras;
	}

	Background Scene::getBackground()
	{
		return this->background;
	}

	string Scene::getName()
	{
		return this->name;
	}

	void Scene::setGameObjectByName(GameObject gameObject, string name)
	{
		if (gameObject.getName() != name)
		{
			throw std::runtime_error("The name of the game object (" + gameObject.getName() + ") must be equal to the name passed as parameter name: " + name);
		} else if (!vectorContains(this->gameObjectsNames, name))
		{
			throw std::runtime_error("There aren't objects with name " + name);
		}
		else
		{
			this->mapNamesGameObjects[name] = gameObject;

			for (int i = 0; i < this->gameObjects.size(); i++)
			{
				if (this->gameObjects[i].getName() == name)
				{
					this->gameObjects[i] = gameObject;
				}
			}
		}
	}

	void Scene::setCamera(int cameraPos)
	{
		this->camera = this->availableCameras[cameraPos];
	}

	void Scene::modifyCamera(Camera camera, unsigned int pos)
	{
		if (pos < this->availableCameras.size())
		{
			throw std::runtime_error("The position of the camera is invalid");
		}

		this->availableCameras[pos] = camera;
	}

	void Scene::setBackground(Background background)
	{
		this->background = background;
	}
}
