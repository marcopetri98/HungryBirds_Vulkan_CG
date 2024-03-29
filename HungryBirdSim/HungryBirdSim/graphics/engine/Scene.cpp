
#include <stdexcept>

#include "DummyRecursionSolver.hpp"
#include "Scene.hpp"
#include "DirectionalLight.hpp"
#include "PointLight.hpp"
#include "SpotLight.hpp"
#include "AmbientLight.hpp"
#include "HemisphericalLight.hpp"
#include "SphericalLight.hpp"
#include "../../utils/CollectionUtils.hpp"

using collectionutils::vectorContains;

namespace graphics
{
	Scene::Scene()
	{

	}

	Scene::Scene(vector<GameObject*> gameObjects, int camera, vector<Camera*> availableCameras, Background* background, string name, int id, DirectionalLight* dirLight, PointLight* pointLight, SpotLight* spotLight, DiffuseModel diffuseModel, SpecularModel specularModel, AmbientLight* ambientLight, HemisphericalLight* hemisphericLight, SphericalLight* sphericalLight)
	{

		for (GameObject* gameObject : gameObjects)
		{
			if (this->gameObjectsNames.size() != 0 && vectorContains(this->gameObjectsNames, gameObject->getName()))
			{
				throw std::runtime_error("Two objects have the same name, objects ids must be unique");
			}
			else
			{
				this->gameObjectsNames.push_back(gameObject->getName());
				this->gameObjects.push_back(gameObject);
				this->mapNamesGameObjects.insert(std::pair<string, GameObject*>(gameObject->getName(), gameObject));
			}
		}

		this->camera = availableCameras[camera];
		this->availableCameras = availableCameras;
		this->background = background;
		this->name = name;
		this->id = id;
		this->dirLight = dirLight;
		this->pointLight = pointLight;
		this->spotLight = spotLight;
		this->diffuseModel = diffuseModel;
		this->specularModel = specularModel;
		this->ambientLight = ambientLight;
		this->hemisphericalLight = hemisphericLight;
		this->sphericalLight = sphericalLight;
	}

	GameObject* Scene::getGameObjectPointerByPos(unsigned int pos)
	{
		if (pos >= this->gameObjects.size())
		{
			throw std::runtime_error("There isn't a game object with position " + std::to_string(pos));
		}

		return this->gameObjects[pos];
	}

	GameObject Scene::getGameObjectByName(string name)
	{
		if (!vectorContains(this->gameObjectsNames, name))
		{
			throw std::runtime_error("There isn't a game object with name: " + name);
		}
		else
		{
			return *this->mapNamesGameObjects[name];
		}
	}

	GameObject* Scene::getGameObjectPointerByName(string name)
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

	void Scene::useDirectionalLight() {
		DirectionalLight* buffer = lastdirLight;
		lastdirLight = dirLight;
		dirLight = buffer;
	}


	void Scene::usePointLight() {
		PointLight* buffer = lastpointLight;
		lastpointLight = pointLight;
		pointLight = buffer;
	}

	void Scene::useSpotLight() {
		SpotLight* buffer = lastspotLight;
		lastspotLight = spotLight;
		spotLight = buffer;
	}

	void Scene::useAmbientLight() {
		AmbientLight* buffer = lastambientLight;
		lastambientLight = ambientLight;
		ambientLight = buffer;
	}

	int Scene::getNumOfGameObjects()
	{
		return this->gameObjects.size();
	}

	vector<GameObject*> Scene::getAllGameObjectsByObjTexPaths(string objPath, string texPath)
	{
		vector<GameObject*> objTexGameObjects;

		objTexGameObjects.clear();
		for (GameObject* objPtr : this->gameObjects)
		{
			if (objPtr->getObjectPath() == objPath && objPtr->getTexturePath() == texPath)
			{
				objTexGameObjects.push_back(objPtr);
			}
		}

		return objTexGameObjects;
	}

	vector<GameObject*> Scene::getAllGameObjects()
	{
		return this->gameObjects;
	}

	DirectionalLight* Scene::getDirectionalLightPointer()
	{
		return this->dirLight;
	}

	PointLight* Scene::getPointLightPointer()
	{
		return this->pointLight;
	}

	SpotLight* Scene::getSpotLightPointer()
	{
		return this->spotLight;
	}

	DiffuseModel Scene::getDiffuseModel()
	{
		return this->diffuseModel;
	}

	SpecularModel Scene::getSpecularModel()
	{
		return this->specularModel;
	}

	AmbientLight* Scene::getAmbientLightPointer()
	{
		return this->ambientLight;
	}

	HemisphericalLight* Scene::getHemisphericalLightPointer()
	{
		return this->hemisphericalLight;
	}

	SphericalLight* Scene::getSphericalLightPointer()
	{
		return this->sphericalLight;
	}

	int Scene::getCurrCameraIdx() {
		return this->currCameraIdx;
	}


	Camera* Scene::getCamera()
	{
		return this->camera;
	}

	vector<Camera*> Scene::getAvailableCameras()
	{
		return this->availableCameras;
	}

	Background* Scene::getBackgroundPointer()
	{
		return this->background;
	}

	Background Scene::getBackground()
	{
		return *this->background;
	}

	string Scene::getName()
	{
		return this->name;
	}

	int Scene::getId()
	{
		return this->id;
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
			for (int i = 0; i < this->gameObjects.size(); i++)
			{
				if (this->gameObjects[i]->getName() == name)
				{
					*this->gameObjects[i] = gameObject;
				}
			}
		}
	}

	void Scene::setDirectionalLight(DirectionalLight* directionalLight)
	{
		this->dirLight = directionalLight;
	}

	void Scene::setPointLight(PointLight* pointLight)
	{
		this->pointLight = pointLight;

	}

	void Scene::setSpotLight(SpotLight* spotLight)
	{
		this->spotLight = spotLight;

	}

	void Scene::setAmbientLight(AmbientLight* ambientLight)
	{
		this->ambientLight = ambientLight;

	}

	void Scene::setHemisphericLight(HemisphericalLight* hemisphericalLight)
	{
		this->hemisphericalLight = hemisphericalLight;

	}

	void Scene::setSphericalLight(SphericalLight* sphericalLight)
	{
		this->sphericalLight = sphericalLight;
	}

	void Scene::setCurrCameraIdx(int idx) {
		this->currCameraIdx = idx;
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

		*this->availableCameras[pos] = camera;
	}

	void Scene::setBackground(Background* background)
	{
		this->background = background;
	}

	void Scene::setDiffuseModel(DiffuseModel diffuseModel) {
		this->diffuseModel = diffuseModel;
	}
	
	void Scene::setSpecularModel(SpecularModel specularModel) {
		this->specularModel = specularModel;
	}
}
