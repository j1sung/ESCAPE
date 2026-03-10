#pragma once

#include "Actor/Actor.h"

using namespace Escape;

class Wall : public Actor
{
	RTTI_DECLARATIONS(Wall, Actor)

public:
	Wall(const Vector2& position);
};

