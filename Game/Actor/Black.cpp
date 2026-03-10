#include "Black.h"

Black::Black(const Vector2& position)
	:super(" ", position, Color::Black)
{
	sortingOrder = 0;
}

Black::~Black()
{
}
