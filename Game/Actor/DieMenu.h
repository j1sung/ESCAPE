#pragma once

#include "Actor/Actor.h"
#include "Data/GameData.h"

using namespace Escape;

class DieMenu : public Actor
{
	RTTI_DECLARATIONS(DieMenu, Actor)

public:
	// Todo: DieMenu ±¸Çö
	DieMenu(GameData data);
	~DieMenu();

};

