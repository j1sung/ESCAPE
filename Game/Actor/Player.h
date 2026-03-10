#pragma once

#include "Actor/Actor.h"
#include "Util/Timer.h"

using namespace Escape;

class ICanMove;

class Player : public Actor
{
	// 발사 모드.
	enum class FireMode
	{
		None = -1,
		OneShot,
		Repeat
	};

	RTTI_DECLARATIONS(Player, Actor)

public:
	Player();
	~Player();

private:
	virtual void Tick(float deltaTime) override;

	// 위로 이동하는 함수.
	void MoveUp(ICanMove* canMove);

	// 아래로 이동하는 함수.
	void MoveDown(ICanMove* canMove);

	// 오른쪽으로 이동하는 함수.
	void MoveRight(ICanMove* canMove);

	// 왼쪽으로 이동하는 함수.
	void MoveLeft(ICanMove* canMove);

	// 탄약 발사 함수.
	void Fire();

	// 연속 발사 함수.
	void FireInterval();

	// 발사 가능여부 확인 함수.
	bool CanShoot() const;

	// Todo: OnDestroy();

private:
	// 발사 모드.
	FireMode fireMode = FireMode::None;

	// 타이머 변수.
	Timer timer;

	// 연사 시간 간격.
	float fireInterval = 0.2f;
};

