#pragma once

#include "Level/LevelManager.h"
#include "Level/Level.h"
#include "Navigation/AStar.h"
#include "Util/Timer.h"
#include "Math/Color.h"
#include "Math/Vector2.h"

#include <vector>

using namespace EscapeEngine;

class GameLevel : public Level
{
	RTTI_DECLARATIONS(GameLevel, Level)

	enum class ResultState
	{
		None = 0,
		GameOver,
		GameClear,
		StageClear,
	};

public:
	GameLevel();
	~GameLevel();

private:

	// 이벤트 함수 오버라이드.
	virtual void Tick(float deltaTime) override;
	virtual void Draw() override;

	// 화이트 스페이스도 맵으로 통째로 찍기.
	void LoadMap(const char* filename);

	// 점수 보여주는 함수.
	void ShowUI();

	// 게임을 처음부터 다시 시작하는 함수.
	void RetryGame();

private:

	// 적 확인 변수.
	bool hadEnemy = false;

	// 점수 UI 문자열.
	char scoreString[128] = {};

	// 조작 UI 문자열.
	char inputString[128] = {};

	// 스테이지 UI 문자열.
	char stageString[128] = {};

	// 타이머 UI 문자열.
	char timerString[128] = {};

	// 결과 UI 문자열.
	char resultString[128] = {};

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

	// 현재 스테이지의 초기 맵 데이터.
	std::vector<std::vector<int>> initialGrid;

	// 시각화용 맵 데이터.
	std::vector<std::vector<int>> displayGrid;

	// 시작 위치.
	Vector2 startPosition = Vector2::Zero;

	// 목표 위치.
	Vector2 goalPosition = Vector2::Zero;

	// 현재 스테이지의 초기 시작 위치.
	Vector2 initialStartPosition = Vector2::Zero;

	// 현재 스테이지의 초기 목표 위치.
	Vector2 initialGoalPosition = Vector2::Zero;

	// 맵 크기.
	int mapWidth = 0;
	int mapHeight = 0;

	// 경로 탐색기.
	AStar aStar;

	// 최종 경로.
	std::vector<Node*> path;

	// 최종 경로 애니메이션 인덱스.
	int pathIndex = 0;

	// 탐색 애니메이션 타이머.
	Timer searchTimer = Timer(0.03f);

	// 경로 애니메이션 타이머.
	Timer pathTimer = Timer(0.05f);

	// 경로 탐색 실행 여부.
	bool hasPathSearched = false;

	// 경로 탐색 성공 여부.
	bool hasPath = false;

	// 탐색 중인지 확인.
	bool isSearching = false;

	// 최종 경로 애니메이션 중인지 확인.
	bool isPathAnimating = false;

	// 위험 지형을 밟았는지 확인.
	bool hasSteppedOnDanger = false;

	// 결과 화면 상태.
	ResultState resultState = ResultState::None;

	// 결과 메뉴 현재 인덱스.
	int resultMenuIndex = 0;

	// 결과 메뉴 선택 색상
	Color selectedColor = Color::Blue;

	// 결과 메뉴 비선택 색상
	Color unSelectedColor = Color::White;

	// 제한 시간.
	float gameTimeLimit = 60.0f;

	// 남은 시간.
	float remainingTime = gameTimeLimit;

	// 스테이지 클리어 시간 보상.
	float stageClearTimeBonus = 10.0f;

	// 스테이지 클리어 연출 타이머.
	Timer stageClearTimer = Timer(2.0f);

	// 안내 문구 버퍼.
	char stateString[128] = {};
};
