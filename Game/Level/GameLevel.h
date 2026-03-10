#pragma once

#include "Level/LevelManager.h"
#include "Level/Level.h"
#include "Interface/ICanMove.h"
#include "Data/GameData.h"
#include "Math/Vector2.h"

using namespace Escape;

class GameLevel : public Level, public ICanMove
{
	RTTI_DECLARATIONS(GameLevel, Level)

public:
	GameLevel();
	~GameLevel();

	bool IsInWhiteSpaceAt(const Actor& actor, const Vector2& newPosition) const;

private:

	// 이벤트 함수 오버라이드.
	virtual void Tick(float deltaTime) override;
	virtual void Draw() override;

	// ----- 게임 내부 액터 충돌 판정 처리 함수 -----
	
	// 화이트 스페이스 액터 범위 처리 함수.
	//void ProcessCollisionWithWhiteSpace(); // 화이트 스페이스와의 충돌 처리(적, 플레이어)

	// 화이트 스페이스도 맵으로 통째로 찍기.
	void LoadMap(const char* filename);

	// 액터 이동가능한지 판단하는 함수.
	virtual bool CanMove(Actor& mover,const Vector2& nextPos) override;
	
	// 공격 충돌 판정 처리 함수.
	
	// [플레이어 공격]: 플레이어 발사체-> 적 body
	void ProcessCollisionPlayerAttackAndEnemy(); 

	// [플레이어 수집]: 플레이어 body -> 적 body(죽었을때만)
	void ProcessCollisionPlayerAndEnemyIsDead();

	// [플레이어 스테이지 클리어]: 플레이어 body -> 벽 body(클리어시만)
	void ProcessCollisionPlayerAndWall();

	// [적 공격]: 적 body(살아있을때만) -> 플레이어 body - (타격후 3초간 무적)
	void ProcessCollisionPlayerAndEnemyIsAlive();

	// [적 이동]: 화이트 스페이스 범위에서만 이동 가능
	void ProcessCollisionEnemyAndWhieSpace();


	// 점수 보여주는 함수.
	void ShowUI();

private:

	// UI로 띄울 게임 데이터.
	//GameData data;

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
};

