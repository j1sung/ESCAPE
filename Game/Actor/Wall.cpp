#include "Actor/Wall.h"

Wall::Wall(const Vector2& position)
	:super("#", position, Color::Green)
{
	sortingOrder = 0;
}
