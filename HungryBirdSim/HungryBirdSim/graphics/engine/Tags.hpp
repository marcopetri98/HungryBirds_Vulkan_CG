#pragma once

#ifndef TAGS_H_
#define TAGS_H_

#include <vector>
using std::vector;

namespace tags
{
	enum class Tag
	{
		RIGID_COLLIDABLE_OBJECT,
		MOVABLE_COLLIDABLE_OBJECT,
		GROUND,
		ENEMY_OBJ,
		COLLECTABLE_OBJ
	};

	int getTags(vector<Tag> tags);

}

#endif
