#pragma once

#include "Engine/Engine.h"
#include "Level/Level.h"

#include <vector>

using namespace EscapeEngine;

enum class State
{ 
	Menu = 0,
	GamePlay = 1,
};

class LevelManager
{
public:

	void Initialize(Engine* engine);
	void RegisterLevel(State state, Level* level);
	void SetState(State state, bool reset = false);
	void ToggleMenu();
	void QuitGame();

	static LevelManager& Get();

private:
	// 엔진 접근 변수
	Engine* engine = nullptr;

	// 게임에서 관리하는 레벨을 저장할 배열.
	std::vector<Level*> levels;

	// 현재 활성화된 레벨을 나타내는 상태 변수
	State current = State::GamePlay;
};
