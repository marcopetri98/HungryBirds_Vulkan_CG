#include <string>

#include "Tags.hpp"

namespace tags
{
	int getTags(vector<Tag> tags)
	{
		int res = 0;
		for (Tag tag : tags) {
			switch (tag)
			{
			case Tag::RIGID_COLLIDABLE_OBJECT:
				res += 0;
				break;

			case Tag::MOVABLE_COLLIDABLE_OBJECT:
				res += 2;
				break;

			case Tag::ENEMY_OBJ:
				res += 4;
				break;

			case Tag::COLLECTABLE_OBJ:
				res += 8;
				break;

			case Tag::GROUND:
				res += 16;
				break;
			}
		}
		return res;
	}
}
