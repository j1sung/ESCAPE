#include "Level/LevelManager.h"
#include "Level/MenuLevel.h"
#include "Core/Input.h"
#include "Render/Renderer.h"

#include <iostream>


MenuLevel::MenuLevel()
{
	// 메뉴 아이템 생성.
	items.emplace_back(new MenuItem("Start Game",
		[]()
		{
			// 메뉴 토글 함수 호출.
			//LevelManager::Get().ToggleMenu();
			LevelManager::Get().SetState(State::GamePlay, true);

		}
	));

	items.emplace_back(new MenuItem("Quit Game",
		[]()
		{
			// 게임 종료
			LevelManager::Get().QuitGame();
		}
	));
}


MenuLevel::~MenuLevel()
{
	// 메뉴 아이템 제거.
	for (MenuItem*& item : items)
	{
		SafeDelete(item);
	}

	// 배열 초기화.
	items.clear();
}

void MenuLevel::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	// 입력 처리 (방향키 위/아래키, 엔터키, ESC).
	// 배열 길이.
	static int length = static_cast<int>(items.size());
	if (Input::Get().GetKeyDown(VK_UP))
	{
		// 인덱스 돌리기 (위 방향으로).
		currentIndex = (currentIndex - 1 + length) % length;
	}
	if (Input::Get().GetKeyDown(VK_DOWN))
	{
		currentIndex = (currentIndex + 1) % length;
	}

	if (Input::Get().GetKeyDown(VK_RETURN))
	{
		// 메뉴 아이템이 저장한 함수 포인터 호출.
		items[currentIndex]->onSelected();
	}
	if (Input::Get().GetKeyDown(VK_ESCAPE))
	{
		// 메뉴씬에서만 ESC 누르면 종료
		currentIndex = 1;
		items[currentIndex]->onSelected();
	}
}

void MenuLevel::Draw()
{
	// 메뉴 제목 출력
	Renderer::Get().Submit("ESCAPE!!", Vector2::Zero);

	// 메뉴 아이템 출력.
	for (int ix = 0; ix < static_cast<int>(items.size()); ++ix)
	{
		// 아이템 색상 확인 (선택 여부 표시).
		Color textColor =
			(ix == currentIndex) ? selectedColor: unSelectedColor;

		Renderer::Get().Submit(items[ix]->text, Vector2(0, 2 + ix), textColor);
	}
}