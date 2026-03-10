#include "vector2.h"
#include "Util/Util.h"

#include <iostream>

namespace Dark
{
	Vector2 Vector2::Zero(0, 0);

	Vector2::Vector2()
	{
	}
	Vector2::Vector2(int x, int y)
		: x(x), y(y)
	{
	}
	Vector2::~Vector2()
	{
		SafeDeleteArray(string);
	}
	const char* Vector2::ToString()
	{
		// 근데 이거 이중해제 문제가 있다고 계속 그래서 나중에 알아봐야할듯.
		// 기존 문자열이 있다면 제거.
		SafeDeleteArray(string);

		string = new char[128];
		memset(string, 0, sizeof(char) * 128);
		sprintf_s(string, 128, "(%d, %d)", x, y);

		return nullptr;
	}
	Vector2 Vector2::operator+(const Vector2& other) const
	{
		return Vector2(x + other.x, y + other.y);
	}
	Vector2 Vector2::operator-(const Vector2& other) const
	{
		return Vector2(x - other.x, y - other.y);
	}
	bool Vector2::operator==(const Vector2& other) const
	{
		return (x == other.x) && (y == other.y);
	}
	bool Vector2::operator!=(const Vector2& other) const
	{
		return !(*this == other);
	}
	Vector2::operator COORD() const
	{
		COORD coord = {};
		coord.X = static_cast<short>(x);
		coord.Y = static_cast<short>(y);

		return coord;
	}
}