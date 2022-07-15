#include <array>
#include <vector>
#include "PlaneCollider3d.hpp"

using std::copy, std::begin, std::end, std::vector;
using graphics::GameObject;

namespace graphics {
	PlaneCollider3D::PlaneCollider3D(GameObject* gameObject, float size_x, float size_z) {
		this->gameObject = gameObject;
		this->size_x = size_x;
		this->size_z = size_z;
		createBoundingBox();
	}


	float PlaneCollider3D::getSize() {
		//TODO wrong
		return this->size_x / 2.0f;
	}

	array<vec3, 4> PlaneCollider3D::getBoundingBox()
	{
		array<vec3, 4> boundingBox = this->boundingBox;
		return boundingBox;
	}

	void PlaneCollider3D::createBoundingBox() {
		vec3 goPos = this->gameObject->getCurrentPos();
		vector<vec3> vertices;
		for (int x = -1; x <= 1; x += 2) {
				for (int z = -1; z <= 1; z += 2) {
					vec3 vertex = goPos + vec3(x * this->size_x / 2.0f, 0, z * this->size_z / 2.0f);
					vertices.push_back(vertex);
				}
		}
		for (int i = 0; i < 4; i++) {
			this->boundingBox[i] = vertices[i];
		}
		this->x_low = this->boundingBox[0].x;
		this->x_high = this->boundingBox[3].x;
		this->z_low = this->boundingBox[0].z;
		this->z_high = this->boundingBox[3].z;
	}

	void PlaneCollider3D::setGameObject(GameObject* gameObject)
	{
		this->gameObject = gameObject;
	}

	GameObject PlaneCollider3D::getGameObject()
	{
		return *this->gameObject;
	}

	bool PlaneCollider3D::checkCollision(vec3 point) {
		return point.y < this->boundingBox[0].y;
	}
}
