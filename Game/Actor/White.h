#pragma once

#include "Actor/Actor.h"

using namespace Escape;

class White : public Actor
{
	RTTI_DECLARATIONS(White, Actor)

public:
	White(const Vector2& position);
	~White();
	virtual void BeginPlay() override;

private:
	//virtual void Tick(float deltaTime) override;
};

