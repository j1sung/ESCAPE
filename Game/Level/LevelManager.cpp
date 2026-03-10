#include "LevelManager.h"
#include "Level/GameLevel.h"

LevelManager& LevelManager::Get()
{
	static LevelManager instance;
	return instance;
}

void LevelManager::Initialize(Engine* engine)
{
	this->engine = engine;
}

void LevelManager::RegisterLevel(State state, Level* level)
{
	// 벡터 특정 인덱스값에 값 저장하기 위해 인덱스 접근
	// ex) MenuLevel = 0, GameLevel = 1
	const int idx = static_cast<int>(state);
	if (levels.size() <= idx)
	{
		levels.resize(idx + 1, nullptr);
	}
	levels[idx] = level;
}

void LevelManager::SetState(State state, bool reset)
{
	current = state;
	const int idx = static_cast<int>(state);

	if (reset && state == State::GamePlay)
	{
		// 기존 레벨 삭제 후 새로 생성
		if (idx < static_cast<int>(levels.size()) && levels[idx])
		{
			delete levels[idx];
			levels[idx] = nullptr;
		}
		levels[idx] = new GameLevel();
	}

	if (engine && idx < static_cast<int>(levels.size()) && levels[idx])
	{
		engine->SetNewLevel(levels[idx]);
	}
}

void LevelManager::ToggleMenu()
{
	if (current == State::GamePlay)
	{
		SetState(State::Menu);
	}
	else
	{
		SetState(State::GamePlay);
	}
}

void LevelManager::QuitGame()
{
	engine->QuitEngine();
}