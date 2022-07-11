#include <string>

#include "Background.hpp"

using std::string;

namespace graphics
{
	Background::Background(string objectPath, string texturePath)
	{
		this->objectPath = objectPath;
		this->texturePath = texturePath;
	}

	string Background::getObjectPath()
	{
		return this->objectPath;
	}

	string Background::getTexturePath()
	{
		return this->texturePath;
	}
}
