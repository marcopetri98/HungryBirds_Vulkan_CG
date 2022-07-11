#pragma once

#ifndef BACKGROUND_HPP
#define BACKGROUND_HPP

#include <string>

using std::string;

namespace graphics
{
	/**
	 * Immutable class representing the background of a scene.
	 */
	class Background
	{
		public:
		/**
		 * Creates a background object.
		 * 
		 * @param objectPath The path to the object file.
		 * @param texturePath The path to the texture file.
		 */
		Background(string objectPath = "none", string texturePath = "none");
		/**
		 * Gets the object path of the background.
		 * 
		 * @return The path to the object.
		 */
		string getObjectPath();
		/**
		 * Gets the texture path of the background.
		 * 
		 * @return The path to the texture.
		 */
		string getTexturePath();

		private:
		/**
		 * The object path for the background.
		 */
		string objectPath;
		/**
		 * The texture path for the background.
		 */
		string texturePath;
	};
}

#endif