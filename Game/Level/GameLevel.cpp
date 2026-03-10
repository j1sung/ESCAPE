#include "Level/GameLevel.h"
#include "Core/Input.h"
#include "Render/Renderer.h"
#include "Engine/Engine.h"

#include <iostream>

/*
0: 빈 공간
1: 벽
2: 시작 위치
3: 목표 위치
4: 위험 지역
5: 최종 경로
*/

GameLevel::GameLevel()
{
	// 임시로 첫 맵 세팅 바로
	// Todo: 이거 스테이지 번호에 따라 다른 파일을 읽게 바꿔도 됨.
	LoadMap("stage1.txt");
}

GameLevel::~GameLevel()
{
}

void GameLevel::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	// 스페이스를 누르면 경로 탐색 시작.
	if (Input::Get().GetKeyDown(VK_SPACE))
	{
		// 탐색할 때마다 원본 맵 기준으로 시각화 맵 초기화.
		displayGrid = grid;

		// 기존 탐색 결과 제거.
		path.clear();

		// 경로 탐색 실행.
		path = aStar.FindPath(startPosition, goalPosition, displayGrid);

		// 탐색 결과 저장.
		hasPathSearched = true;
		hasPath = path.empty() == false;
	}
}

void GameLevel::Draw()
{
	// grid를 직접 렌더링한다.
	// 이번 A* 시각화는 액터를 생성하지 않고 좌표 기반으로 처리한다.
	for (int y = 0; y < mapHeight; ++y)
	{
		for (int x = 0; x < mapWidth; ++x)
		{
			const int cell = displayGrid[y][x];

			const char* text = "  ";
			Color color = Color::White;

			switch (cell)
			{
			case 0:
				text = ". ";
				color = Color::White;
				break;
			case 1:
				text = "# ";
				color = Color::White;
				break;
			case 2:
				text = "S ";
				color = Color::Red;
				break;
			case 3:
				text = "G ";
				color = Color::Green;
				break;
			case 4:
				text = "! ";
				color = Color::Red | Color::BgWhite;
				break;
			case 5:
				text = "+ ";
				color = Color::Green;
				break;
			}

			// 두 칸 단위로 찍어서 grid가 찌그러지지 않게 맞춘다.
			Renderer::Get().Submit(text, Vector2(x * 2, y), color);
		}
	}

	if (isGameOver)
	{

		if (stageNum == 4)
		{
			// 게임 클리어 UI 표시
			// 화면 지우고 클리어 UI 표시
		}
		else
		{
			// 게임 오버 UI 표시
			// 화면 지우고 현재 스테이지 정보랑 게임오버 메시지 출력
		}

		// MenuLevel처럼 메뉴 선택 UI 띄워서 고를 수 있게 함.
	}

	ShowUI();
}

void GameLevel::LoadMap(const char* filename)
{
	// 파일 로드.
	// 최종 파일 경로 만들기.
	char path[2048] = {};
	sprintf_s(path, 2048, "../Assets/%s", filename);

	// 파일 열기.
	FILE* file = nullptr;
	fopen_s(&file, path, "rt");

	// 예외 처리.
	if (!file)
	{
		std::cerr << "Error: Failed to open map file.\n";
		__debugbreak();
	}

	// 기존 맵 데이터 초기화.
	grid.clear();
	displayGrid.clear();
	startPosition = Vector2::Zero;
	goalPosition = Vector2::Zero;
	bool hasStartPosition = false;
	bool hasGoalPosition = false;

	// 한 줄씩 읽어서 2차원 grid로 변환.
	char buffer[2048] = {};
	int expectedWidth = -1;
	int y = 0;
	while (fgets(buffer, sizeof(buffer), file))
	{
		std::vector<int> row;

		for (int x = 0; buffer[x] != '\0'; ++x)
		{
			char mapCharacter = buffer[x];

			// 개행 문자 처리.
			if (mapCharacter == '\n' || mapCharacter == '\r')
			{
				continue;
			}

			/*
			0: 빈 공간
			1: 벽
			2: 시작지점
			3: 목적지
			4: 위험 지역
			*/

			// 공백으로 비워둔 맵은 빈 공간으로 처리.
			int cell = 0;
			if (mapCharacter >= '0' && mapCharacter <= '4')
			{
				cell = mapCharacter - '0';
			}
			else if (mapCharacter == ' ')
			{
				cell = 0;
			}

			// 원본 grid에 저장.
			row.emplace_back(cell);

			// 시작 / 목표 좌표 저장.
			if (cell == 2)
			{
				hasStartPosition = true;
				startPosition = Vector2(static_cast<int>(row.size()) - 1, y);
			}
			else if (cell == 3)
			{
				hasGoalPosition = true;
				goalPosition = Vector2(static_cast<int>(row.size()) - 1, y);
			}
		}

		if (row.empty() == false)
		{
			// 모든 줄 길이는 동일해야 한다.
			if (expectedWidth < 0)
			{
				expectedWidth = static_cast<int>(row.size());
			}
			else if (expectedWidth != static_cast<int>(row.size()))
			{
				std::cerr << "Error: Invalid map width.\n";
				__debugbreak();
			}

			grid.emplace_back(row);
			++y;
		}
	}

	// 시작 / 목표 지점은 맵 파일에 반드시 있어야 한다.
	if (grid.empty() == false &&
		(!hasStartPosition || !hasGoalPosition))
	{
		std::cerr << "Error: Failed to find start / goal position.\n";
		__debugbreak();
	}

	// 시각화용 grid는 원본 맵을 그대로 복사해서 시작.
	displayGrid = grid;
	mapHeight = static_cast<int>(displayGrid.size());
	mapWidth = mapHeight > 0 ? static_cast<int>(displayGrid[0].size()) : 0;

	// 파일 닫기.
	fclose(file);
}

bool GameLevel::CanMove(Actor& mover, const Vector2& nextPos)
{
	// 범위를 벗어나면 이동 불가.
	if (nextPos.x < 0 || nextPos.x >= mapWidth || nextPos.y < 0 || nextPos.y >= mapHeight)
	{
		return false;
	}

	// 벽은 통과할 수 없다.
	return grid[nextPos.y][nextPos.x] != 1;
}

void GameLevel::ShowUI()
{
	sprintf_s(scoreString, 128, "Space : Find Path");
	Renderer::Get().Submit(scoreString, Vector2(0, mapHeight + 1), Color::White);

	if (!hasPathSearched)
	{
		sprintf_s(stateString, 128, "Status : Ready");
	}
	else if (hasPath)
	{
		sprintf_s(stateString, 128, "Status : Path Found (%d)", static_cast<int>(path.size()));
	}
	else
	{
		sprintf_s(stateString, 128, "Status : Path Not Found");
	}

	Renderer::Get().Submit(stateString, Vector2(0, mapHeight + 2), Color::White);
}
