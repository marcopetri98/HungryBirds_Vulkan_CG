
#include "GameObject.hpp"

namespace graphics
{
	GameObject::GameObject(string name = "none", int tag = 0, string objectPath = "none", string texturePath = "none", mat4 initialTransform = mat4(1), int layer = 0)
	{
		this->name = string(name);
		this->tag = tag;
		this->layer = layer;
		this->objectPath = string(objectPath);
		this->texturePath = string(texturePath);
		this->initialTransform = initialTransform;
		this->currentTransform = initialTransform;
		this->collider = NULL;
	}

	void GameObject::setCollider(Collider* collider)
	{
		this->collider = collider;
	}

	void GameObject::setCurrentTransform(mat4 transform)
	{
		this->currentTransform = transform;
	}

	string GameObject::getName()
	{
		return string(this->name);
	}

	int GameObject::getTag()
	{
		return this->tag;
	}

	int GameObject::getLayer()
	{
		return this->layer;
	}

	mat4 GameObject::getInitialTransform()
	{
		return this->initialTransform;
	}

	mat4 GameObject::getCurrentTransform()
	{
		return this->currentTransform;
	}

	string GameObject::getObjectPath()
	{
		return this->objectPath;
	}

	string GameObject::getTexturePath()
	{
		return this->texturePath;
	}

	Collider* GameObject::getCollider()
	{
		return this->collider;
	}
}
