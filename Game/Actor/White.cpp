#include "White.h"
#include "Util/Util.h"
#include "Core/Input.h"
#include "Level/Level.h"
#include "Actor/EnemySpawner.h"

namespace
{
	// 3/4/5줄짜리 이미지 텍스트. 필요에 맞게 내용만 교체하세요.
	static const char* const kWhiteImages[] =
	{
		"         \n         \n         ",
		"          \n          \n          \n          \n          ",
		"       \n       \n       \n       \n       \n       ",
	};

	const char* PickRandomWhiteImage()
	{
		const int count = static_cast<int>(sizeof(kWhiteImages) / sizeof(kWhiteImages[0]));
		const int index = Util::Random(0, count - 1);
		return kWhiteImages[index];
	}
}

White::White(const Vector2& position)
	:super(PickRandomWhiteImage(), position, Color::BgWhite)
{
	sortingOrder = 0;
}

White::~White()
{
}

void White::BeginPlay()
{
	super::BeginPlay();
	// \n 도달까지 우측 끝 x좌표.
	const int rightX = GetRightmostX();
	GetOwner()->AddNewActor(new EnemySpawner(GetPosition(), rightX));
}

//void White::Tick(float deltaTime)
//{
//	if (Input::Get().GetKeyDown(VK_SPACE))
//	{
//		ChangeImage(PickRandomWhiteImage());
//	}
//}
