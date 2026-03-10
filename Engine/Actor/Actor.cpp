#include "Actor/Actor.h"
#include "Util/Util.h"
#include "Render/Renderer.h"
#include "Engine/Engine.h"

#include <iostream>
#include <Windows.h>

namespace Dark
{
	static void ComputeImageSize(const char* image, int& outWidth, int& outHeight)
	{
		outWidth = 0;
		outHeight = 0;

		if (!image || image[0] == '\0')
		{
			return;
		}

		int currentLine = 0;
		const char* p = image;
		while (true)
		{
			if (*p == '\n' || *p == '\0')
			{
				if (currentLine > outWidth)
				{
					outWidth = currentLine;
				}
				outHeight++;

				if (*p == '\0')
				{
					break;
				}
				currentLine = 0;
			}
			else
			{
				currentLine++;
			}
			++p;
		}
	}

	Actor::Actor(const char* image, const Vector2& position, Color color)
		:position(position), color(color)
	{
		// 문자열 복사.
		int imageLength = static_cast<int>(strlen(image));
		this->image = new char[imageLength + 1];
		strcpy_s(this->image, imageLength + 1, image);

		ComputeImageSize(this->image, actorWidth, actorHeight);
	}

	Actor::~Actor()
	{
		// 메모리 해제
		SafeDeleteArray(image);
	}
	void Actor::BeginPlay()
	{
		// 이벤트를 받은 후에는 플래그 설정.
		hasBeganPlay = true;
	}

	void Actor::Tick(float deltaTime)
	{
	}

	void Actor::Draw()
	{
		// 렌더러에 데이터 제출.
		Renderer::Get().Submit(image, position, color, sortingOrder);
	}

	void Actor::Destroy()
	{
		// 삭제 플래그 설정.
		destroyRequested = true;

		// 삭제 이벤트 호출.
		OnDestroy();
	}

	void Actor::OnDestroy()
	{
	}

		bool Actor::TestIntersect(const Actor* const other)
	{
		// AABB (Axis Aligned Bounding Box).
		// x, y ?? ??.

		int xMin = position.x;
		int xMax = position.x + actorWidth - 1;
		int yMin = position.y;
		int yMax = position.y + actorHeight - 1;

		int otherXMin = other->GetPosition().x;
		int otherXMax = other->GetPosition().x + other->actorWidth - 1;
		int otherYMin = other->GetPosition().y;
		int otherYMax = other->GetPosition().y + other->actorHeight - 1;

		if (otherXMin > xMax) return false;
		if (otherXMax < xMin) return false;
		if (otherYMin > yMax) return false;
		if (otherYMax < yMin) return false;

		return true;
	}

int Actor::GetRightmostX() const
{
	if (!image)
	{
		return position.x;
	}

	int maxLine = 0;
	const char* lineStart = image;
	while (lineStart)
	{
		const char* lineEnd = strchr(lineStart, '\n');
		const int lineLength = lineEnd
			? static_cast<int>(lineEnd - lineStart)
			: static_cast<int>(strlen(lineStart));

		if (lineLength > maxLine)
		{
			maxLine = lineLength;
		}

		if (!lineEnd)
		{
			break;
		}

		lineStart = lineEnd + 1;
	}

	if (maxLine <= 0)
	{
		return position.x;
	}

	return position.x + maxLine - 1;
}

	void Actor::ChangeImage(const char* newImage)
	{
		// 기존 메모리 해제.
		SafeDeleteArray(image);

		// 새로운 문자열 복사.
		int imageLength = static_cast<int>(strlen(newImage));
		image = new char[imageLength + 1];
		strcpy_s(image, imageLength + 1, newImage);

		ComputeImageSize(image, actorWidth, actorHeight);
	}

	void Actor::SetPosition(const Vector2& newPosition)
	{
		// 변경하려는 위치가 현재 위치와 같으면 건너뜀.
		if (position == newPosition)
		{
			return;
		}

		// 새로운 위치 설정.
		position = newPosition;
	}
}