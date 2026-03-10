#pragma once

#include "Actor/Actor.h"

using namespace Dark;

class Wall : public Actor
{
	RTTI_DECLARATIONS(Wall, Actor)

public:
	Wall(const Vector2& position);
};

