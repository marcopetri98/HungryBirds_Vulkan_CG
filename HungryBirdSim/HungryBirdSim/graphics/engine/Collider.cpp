#include "Collider.hpp"

namespace graphics {
	string Collider::getLastCollision() {
		return this->lastCollision;
	}
	
	void Collider::setLastCollision(string name) {
		this->lastCollision = name;
	}

}
