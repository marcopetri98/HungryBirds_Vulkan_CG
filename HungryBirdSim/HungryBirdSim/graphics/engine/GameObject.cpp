
#include "GameObject.hpp"

namespace graphics
{
	GameObject::GameObject(string name, int tag, string objectPath, string texturePath, vec3 initialPos, vec3 initialRot, int layer)
	{
		this->name = string(name);
		this->tag = tag;
		this->layer = layer;
		this->objectPath = string(objectPath);
		this->texturePath = string(texturePath);
		this->initialPos = vec3(initialPos);
		this->initialRot = vec3(initialRot);
		this->currentPos = vec3(initialPos);
		this->currentRot = vec3(initialRot);
		this->collider = NULL;
	}

	void GameObject::setCollider(Collider* collider)
	{
		this->collider = collider;
	}

	void GameObject::setCurrentPos(vec3 pos)
	{
		this->currentPos = vec3(pos);
	}

	void GameObject::setCurrentRot(vec3 rot)
	{
		this->currentRot = vec3(rot);
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

	vec3 GameObject::getInitialPos()
	{
		return vec3(this->initialPos);
	}

	vec3 GameObject::getInitialRot()
	{
		return vec3(this->initialRot);
	}

	vec3 GameObject::getCurrentPos()
	{
		return vec3(this->currentPos);
	}

	vec3 GameObject::getCurrentRot()
	{
		return vec3(this->currentRot);
	}

	string GameObject::getObjectPath()
	{
		return string(this->objectPath);
	}

	string GameObject::getTexturePath()
	{
		return string(this->texturePath);
	}

	Collider* GameObject::getCollider()
	{
		return this->collider;
	}
}
