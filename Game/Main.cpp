#include "Engine/Engine.h"
#include "Level/LevelManager.h"
#include "Level/MenuLevel.h"
#include "Level/GameLevel.h"
#include "Util/Util.h"

#include <iostream>

using namespace Dark;

int main()
{
	Util::SetRandomSeed();
	Engine engine;

	LevelManager::Get().Initialize(&engine);
	LevelManager::Get().RegisterLevel(State::Menu, new MenuLevel());
	LevelManager::Get().RegisterLevel(State::GamePlay, new GameLevel());
	LevelManager::Get().SetState(State::Menu);

	engine.Run();

	return 0;
}