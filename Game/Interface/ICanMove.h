#pragma once

#include "Actor/Actor.h"
#include "Math/Vector2.h"

// 액터가 이동할 위치를 판단해주는 인터페이스.
class ICanMove
{
public:
	// 이동가능한지 문의할 때 사용할 함수.
	// currentPos: 현재 위치
	// nextPos: 다음 이동 위치
	virtual bool CanMove(
		Escape::Actor& mover,
		const Escape::Vector2& nextPos
	) = 0;
};