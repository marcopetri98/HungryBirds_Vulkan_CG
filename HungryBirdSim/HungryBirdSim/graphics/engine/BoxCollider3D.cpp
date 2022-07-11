#include <array>
#include <vector>
#include "BoxCollider3D.hpp"

using std::copy, std::begin, std::end, std::vector;
using graphics::GameObject;

namespace graphics {
	BoxCollider3D::BoxCollider3D(GameObject gameObject, float size_x, float size_y, float size_z) {
		this->gameObject = gameObject;
		this->size_x = size_x;
		this->size_y = size_y;
		this->size_z = size_z;
		createBoundingBox();
	}

	BoxCollider3D::BoxCollider3D(GameObject gameObject, float side, float height) {
		BoxCollider3D(gameObject, side, height, side);
	}

	BoxCollider3D::BoxCollider3D(GameObject gameObject, float side) {
		BoxCollider3D(gameObject, side, side, side);
	}


	array<vec3, 8> BoxCollider3D::getBoundingBox()
	{
		array<vec3, 8> boundingBox = this->boundingBox;
		return boundingBox;
	}

	void BoxCollider3D::createBoundingBox() {
		vec3 goPos = this->gameObject.getCurrentPos();
		vector<vec3> vertices;
		for (int x = -1; x <= 1; x+=2) {
			for (int y = -1; y <= 1; y+=2) {
				for (int z = -1; z <= 1; z++) {
					vec3 vertex = goPos + vec3(x*this->size_x/2.0f, y*this->size_y/2.0f, z*this->size_z/2.0f);
					vertices.push_back(vertex);
				}
			}
		}
		for (int i = 0; i < 8; i++) {
			this->boundingBox[i] = vertices[i];
		}
		this->x_low = this->boundingBox[0].x;
		this->x_high = this->boundingBox[7].x;
		this->y_low = this->boundingBox[0].y;
		this->y_high = this->boundingBox[7].y;
		this->z_low = this->boundingBox[0].z;
		this->z_high = this->boundingBox[7].z;
	}

	GameObject BoxCollider3D::getGameObject()
	{
		return this->gameObject;
	}

	bool BoxCollider3D::checkCollision(vec3 point) {
		return	this->x_low <= point.x && point.x <= this->x_high &&
				this->y_low <= point.y && point.y <= this->y_high &&
				this->z_low <= point.z && point.z <= this->z_high;
	}
}
