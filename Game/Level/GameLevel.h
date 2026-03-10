#pragma once

#include "Level/LevelManager.h"
#include "Level/Level.h"
#include "Interface/ICanMove.h"
#include "Navigation/AStar.h"
#include "Data/GameData.h"
#include "Math/Vector2.h"

#include <vector>

using namespace Dark;

class GameLevel : public Level, public ICanMove
{
	RTTI_DECLARATIONS(GameLevel, Level)

public:
	GameLevel();
	~GameLevel();

private:

	// 이벤트 함수 오버라이드.
	virtual void Tick(float deltaTime) override;
	virtual void Draw() override;

	// 화이트 스페이스도 맵으로 통째로 찍기.
	void LoadMap(const char* filename);

	// 액터 이동가능한지 판단하는 함수.
	virtual bool CanMove(Actor& mover, const Vector2& nextPos) override;

	// 점수 보여주는 함수.
	void ShowUI();

private:

	// 적 확인 변수.
	bool hadEnemy = false;

	// 점수 UI 문자열.
	char scoreString[128] = {};

	// 화이트 스페이스인지 확인.
	bool isWhiteSpace = false;

	// 스테이지가 클리어 됐는지 확인.
	bool isStageCleared = false;

	// 플레이어가 죽었는지 확인.
	bool isPlayerDead = false;

	// 죽음 메시지 출력 됐는지 플래그.
	bool deadSubmitted = false;

	// 플레이어가 죽은 위치 (Draw에서 처리하기 위해 Tick에서 저장).
	Vector2 playerDeadPosition;

	bool isGameOver = false;

	int stageNum = 1;

	// 원본 맵 데이터.
	std::vector<std::vector<int>> grid;

	// 시각화용 맵 데이터.
	std::vector<std::vector<int>> displayGrid;

	// 시작 위치.
	Vector2 startPosition = Vector2::Zero;

	// 목표 위치.
	Vector2 goalPosition = Vector2::Zero;

	// 맵 크기.
	int mapWidth = 0;
	int mapHeight = 0;

	// 경로 탐색기.
	AStar aStar;

	// 최종 경로.
	std::vector<Node*> path;

	// 경로 탐색 실행 여부.
	bool hasPathSearched = false;

	// 경로 탐색 성공 여부.
	bool hasPath = false;

	// 안내 문구 버퍼.
	char stateString[128] = {};
};
