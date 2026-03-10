#pragma once

#include "Math/Vector2.h"
#include "Math/Color.h"

#include <time.h>

using namespace Dark;

// 헬퍼(Helper)기능 제공.
namespace Util
{
	// 콘솔 커서 위치 이동(설정)하는 함수.
	// 콘솔 커서: 다음 글자 출력될 문자 좌표(깜빡임)
	inline void SetConsolePosition(const Vector2& position)
	{
		SetConsoleCursorPosition(
			GetStdHandle(STD_OUTPUT_HANDLE),
			static_cast<COORD>(position)
		);
	}

	// 콘솔 텍스트 설정 함수.
	// 콘솔 텍스트가 다시 바꾸기 전까지 이 지정된 색으로 바뀜
	inline void SetConsoleTextColor(Color color)
	{
		SetConsoleTextAttribute(
			GetStdHandle(STD_OUTPUT_HANDLE),
			static_cast<unsigned short>(color)
		);
	}

	// 커서 끄기.
	inline void TurnOffCursor()
	{
		// 커서 끄기
		CONSOLE_CURSOR_INFO info = {};
		GetConsoleCursorInfo(
			GetStdHandle(STD_OUTPUT_HANDLE),
			&info
		);

		info.bVisible = false;
		SetConsoleCursorInfo(
			GetStdHandle(STD_OUTPUT_HANDLE),
			&info
		);
	}

	// 커서 켜기.
	inline void TurnOnCursor()
	{
		// 커서 끄기.
		CONSOLE_CURSOR_INFO info = {};
		GetConsoleCursorInfo(
			GetStdHandle(STD_OUTPUT_HANDLE),
			&info
		);

		info.bVisible = true;
		SetConsoleCursorInfo(
			GetStdHandle(STD_OUTPUT_HANDLE),
			&info
		);
	}

	inline void SetRandomSeed()
	{
		// 시간 값을 랜덤 종자값으로 설정.
		// time(nullptr) → 1970-01-01 00:00:00 (Unix Epoch) 이후 경과한 초(second) 수 반환
		// C 표준 난수 생성기 rand()의 초기 상태(seed)를 설정
		// 같은 seed → 같은 난수 시퀀스
		unsigned int seed = static_cast<unsigned int>(time(nullptr));
		srand(seed);
	}

	// 정수 난수 함수. -> 이해가 더 필요
	inline int Random(int min, int max)
	{
		int diff = (max - min) + 1;
		return ((diff * rand()) / (RAND_MAX + 1)) + min;
	}

	// 부동 소수점 난수 함수.
	inline float RandomRange(float min, float max)
	{
		// 0~1 사이의 float 기반 랜덤 값.
		float random = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

		float diff = (max - min);
		return (random * diff) + min;
	}

	// Clamp: 어떤 값을 두 수 사이로 고정할 때 사용하는 함수.
	template<typename T>
	T Clamp(T value, T min, T max)
	{
		if (value < min)
		{
			value = min;
		}
		else if(value > max)
		{
			value = max;
		}
		return value;
	}

	template<typename T>
	T ClampMax(T value, T max)
	{
		return value > max ? max : value;
	}
}

// 메모리 정리 함수.
template<typename T>
void SafeDelete(T*& t)
{
	if (t)
	{
		delete t;
		t = nullptr;
	}
}

template<typename T>
void SafeDeleteArray(T*& t)
{
	if (t)
	{
		delete[] t;
		t = nullptr;
	}
}