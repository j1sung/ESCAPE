#pragma once

#include "Common/Common.h"

#include <Windows.h>

namespace Dark
{
	// 콘솔에 텍스트 색상 등을 지정할 때 사용할
	// 색상 열거형.
	enum class DARK_API Color : unsigned short
	{
		Black = 0,
		Blue = FOREGROUND_BLUE,
		Green = FOREGROUND_GREEN,
		Red = FOREGROUND_RED,
		White = Blue | Green | Red,

		BgBlue = BACKGROUND_BLUE,
		BgGreen = BACKGROUND_GREEN,
		BgRed = BACKGROUND_RED,
		BgWhite = BgBlue | BgGreen | BgRed,
	};

	inline Color operator|(Color left, Color right)
	{
		return static_cast<Color>(static_cast<unsigned short>(left) | static_cast<unsigned short>(right));
	}
}