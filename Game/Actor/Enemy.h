#pragma once

#include "Actor/Actor.h"
#include "Util/Timer.h"

using namespace Escape;

class Enemy : public Actor
{
	RTTI_DECLARATIONS(Enemy, Actor)

	// 이동 방향 열거형.
	enum class MoveDirection
	{
		None = -1,
		Left,
		Right
	};

public:
	Enemy(const char* image, const Vector2& position, const int rightX);
	~Enemy();

	virtual void Tick(float deltaTime) override;

	// 데미지 처리 함수.
	void OnDamaged();

private:
	// 이동 방향 열거형.
	MoveDirection direction = MoveDirection::None;

	// 좌우 이동 처리를 위한 변수.
	float xPosition = 0.0f;
	float moveSpeed = 0.5f;
	float moveTimer = 0.0f;
	float moveInterval = 0.2f;

	float yPosition = 0.0f;

	// 발사 타이머.
	Timer timer;
};

