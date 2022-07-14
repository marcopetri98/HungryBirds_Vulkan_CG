#include <vector>

#include "GameObject.hpp"
#include "Tags.hpp"

using std::vector;

namespace graphics
{
	GameObject::GameObject()
	{

	}
	
	GameObject::GameObject(string name, vector<Tag> tags, string objectPath, string texturePath, mat4 initialTransform, int layer)
	{
		this->name = string(name);
		this->tag = getTags(tags);
		this->layer = layer;
		this->objectPath = string(objectPath);
		this->texturePath = string(texturePath);
		this->initialTransform = initialTransform;
		this->currentTransform = initialTransform;
		this->collider = NULL;
	}

	void GameObject::setCollider(Collider* collider)
	{
		collider->setGameObject(this);
		this->collider = collider;
	}

	void GameObject::setCurrentTransform(mat4 transform)
	{
		this->currentTransform = transform;
	}

	void GameObject::setVelocity(vec3 velocity) {
		this->velocity = velocity;
	}

	void GameObject::setAcceleration(vec3 acceleration) {
		this->acceleration = acceleration;
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

	vec3 GameObject::getCurrentPos()
	{
		return vec3(this->currentTransform[0][3], this->currentTransform[1][3], this->currentTransform[2][3]);
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
	
	vec3 GameObject::getVelocity() {
		return this->velocity;
	}

	vec3 GameObject::getAcceleration() {
		return this->acceleration;
	}
}
