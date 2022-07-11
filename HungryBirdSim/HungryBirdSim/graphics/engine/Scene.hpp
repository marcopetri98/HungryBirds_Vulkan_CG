#pragma once

#ifndef SCENE_HPP
#define SCENE_HPP

#include <string>
#include <vector>
#include <map>

#include "DummyRecursionSolver.hpp"
#include "GameObject.hpp"
#include "Camera.hpp"
#include "Background.hpp"

using std::string;
using std::vector;
using std::map;

namespace graphics
{
	class Scene
	{
		public:
		/**
		 * Creates the scene.
		 * 
		 * @param gameObjects List of all the game objects to be included in the scene.
		 * @param camera Camera of the scene.
		 * @param background Background of the scene.
		 * @param name Unique name of the scene.
		 * @param id Unique id of the scene.
		 */
		Scene(vector<GameObject> gameObjects, Camera camera, Background background, string name, int id);
		/**
		 * Gets a game object by its name.
		 * 
		 * @param name Name of the game object to retrieve.
		 * @return The GameObject.
		 */
		GameObject getGameObjectByName(string name);
		/**
		 * Gets the scene camera.
		 * 
		 * @return Camera of the scene.
		 */
		Camera getCamera();
		/**
		 * Gets the background of the scene.
		 * 
		 * @return Background of the scene.
		 */
		Background getBackground();
		/**
		 * Gets the name of the scene.
		 * 
		 * @return Name of the scene.
		 */
		string getName();
		/**
		 * Updates the game object by using its name as indexer.
		 * 
		 * @param gameObject The new GameObject that will replace the old one.
		 * @param name The name of the GameObject.
		 */
		void setGameObjectByName(GameObject gameObject, string name);
		/**
		 * Set the camera for the scene.
		 * 
		 * @param camera The camera to be set.
		 */
		void setCamera(Camera camera);
		/**
		 * Set the background for the scene.
		 * 
		 * @param background The background to be set.
		 */
		void setBackground(Background background);

		private:
		/**
		 * Vector of the names of the game objects.
		 */
		vector<string> gameObjectsNames;
		/**
		 * Map between names and game objects.
		 */
		map<string, GameObject> mapNamesGameObjects;
		/**
		 * Vector of all the game objects in the scene.
		 */
		vector<GameObject> gameObjects;
		/**
		 * The camera defining the view.
		 */
		Camera camera;
		/**
		 * The background for the scene.
		 */
		Background background;
		/**
		 * The name of the scene.
		 */
		string name;
		/**
		 * The id of the scene.
		 */
		int id;
	};
}

#endif