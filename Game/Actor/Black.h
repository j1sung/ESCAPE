#pragma once

#include "Actor/Actor.h"

using namespace Escape;

class Black : public Actor
{
	RTTI_DECLARATIONS(Black, Actor)

public:
	Black(const Vector2& position);
	~Black();
};

