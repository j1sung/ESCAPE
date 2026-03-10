#include "Level/GameLevel.h"
#include "Core/Input.h"
#include "Render/Renderer.h"
#include "Engine/Engine.h"

#include <iostream>

namespace
{
	const char* const stageFiles[] =
	{
		"stage1.txt",
		"stage2.txt",
		"stage3.txt"
	};

	const int stageCount = static_cast<int>(sizeof(stageFiles) / sizeof(stageFiles[0]));

	const char* const resultMenuItems[] =
	{
		"Retry",
		"Title"
	};

	const int resultMenuCount = static_cast<int>(sizeof(resultMenuItems) / sizeof(resultMenuItems[0]));
}

/*
0: 빈 공간
1: 벽
2: 시작 위치
3: 목표 위치
4: 위험 지역
5: 탐색 과정
6: 최종 경로
7: 위험 지역을 밟은 경로
8: 외곽 고정 벽
*/

GameLevel::GameLevel()
{
	// 임시로 첫 맵 세팅 바로
	// Todo: 이거 스테이지 번호에 따라 다른 파일을 읽게 바꿔도 됨.
	stageNum = 1;
	remainingTime = gameTimeLimit;
	LoadMap(stageFiles[stageNum - 1]);
}

GameLevel::~GameLevel()
{
}

void GameLevel::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	// 결과 화면에서는 메뉴 입력만 처리한다.
	if (resultState == ResultState::GameOver || resultState == ResultState::GameClear)
	{
		if (Input::Get().GetKeyDown(VK_UP))
		{
			resultMenuIndex = (resultMenuIndex - 1 + resultMenuCount) % resultMenuCount;
		}
		if (Input::Get().GetKeyDown(VK_DOWN))
		{
			resultMenuIndex = (resultMenuIndex + 1) % resultMenuCount;
		}

		if (Input::Get().GetKeyDown(VK_RETURN))
		{
			if (resultMenuIndex == 0)
			{
				RetryGame();
			}
			else if (resultMenuIndex == 1)
			{
				LevelManager::Get().SetState(State::Menu);
			}
		}

		return;
	}

	// 스테이지 클리어 연출 중에는 제한 시간도 멈춘다.
	if (resultState == ResultState::StageClear)
	{
		stageClearTimer.Tick(deltaTime);
		if (stageClearTimer.IsTimeOut())
		{
			stageClearTimer.Reset();
			resultState = ResultState::None;
			++stageNum;
			LoadMap(stageFiles[stageNum - 1]);
		}
		return;
	}

	// 편집 대기 중일 때만 제한 시간을 차감한다.
	if (!isSearching && !isPathAnimating)
	{
		remainingTime -= deltaTime;
		if (remainingTime <= 0.0f)
		{
			remainingTime = 0.0f;
			isSearching = false;
			isPathAnimating = false;
			isGameOver = true;
			resultState = ResultState::GameOver;
			return;
		}
	}

	// 게임 진행 중 ESC를 누르면 타이틀 메뉴로 돌아간다.
	if (Input::Get().GetKeyDown(VK_ESCAPE))
	{
		LevelManager::Get().SetState(State::Menu);
		return;
	}

	// 탐색 중이 아닐 때만 맵 편집 허용.
	// 좌클릭한 위치가 원래 바닥(0) 또는 벽(1)이면 서로 토글한다.
	if (!isSearching && !isPathAnimating && Input::Get().GetMouseButtonDown(0))
	{
		// 경로 탐색에 실패한 뒤 클릭하면 현재 편집된 맵 상태는 유지하고
		// 탐색 흔적만 지운 뒤 다시 편집 / 탐색할 수 있게 한다.
		if (hasPathSearched && !hasPath)
		{
			displayGrid = grid;
			hasPathSearched = false;
			hasPath = false;
			hasSteppedOnDanger = false;
			isGameOver = false;
			resultState = ResultState::None;
			path.clear();
			pathIndex = 0;
			return;
		}

		Vector2 mousePosition = Input::Get().MousePosition();
		const int gridX = mousePosition.x / 2;
		const int gridY = mousePosition.y;

		if (gridX >= 0 && gridX < mapWidth && gridY >= 0 && gridY < mapHeight)
		{
			int& cell = grid[gridY][gridX];

			// 외곽 고정 벽(8)은 편집 불가.
			if (cell == 8)
			{
				return;
			}

			if (cell == 0)
			{
				cell = 1;
			}
			else if (cell == 1)
			{
				cell = 0;
			}

			// 편집된 원본 맵 기준으로 화면도 즉시 갱신.
			displayGrid = grid;
			hasPathSearched = false;
			hasPath = false;
			hasSteppedOnDanger = false;
			isGameOver = false;
			resultState = ResultState::None;
			path.clear();
			pathIndex = 0;
		}
	}

	// 스페이스를 누르면 경로 탐색 시작.
	if (Input::Get().GetKeyDown(VK_SPACE))
	{
		// 탐색할 때마다 원본 맵 기준으로 시각화 맵 초기화.
		displayGrid = grid;

		// 기존 탐색 결과 제거.
		path.clear();
		pathIndex = 0;
		searchTimer.Reset();
		pathTimer.Reset();
		stageClearTimer.Reset();
		hasSteppedOnDanger = false;
		isGameOver = false;
		resultState = ResultState::None;

		// 경로 탐색 시작.
		aStar.Initialize(startPosition, goalPosition);

		// 탐색 상태 초기화.
		hasPathSearched = false;
		hasPath = false;
		isSearching = true;
		isPathAnimating = false;
	}

	// 기존 프로젝트처럼 탐색 과정을 애니메이션으로 보여준다.
	if (isSearching)
	{
		searchTimer.Tick(deltaTime);
		if (searchTimer.IsTimeOut())
		{
			searchTimer.Reset();
			aStar.Step(displayGrid);

			// 목표를 찾았으면 이제 최종 경로만 다시 애니메이션으로 보여준다.
			if (aStar.IsFinished())
			{
				hasPathSearched = true;
				hasPath = aStar.HasFoundPath();
				isSearching = false;

				if (hasPath)
				{
					path = aStar.GetPath();
					displayGrid = grid;
					isPathAnimating = true;
				}
			}
		}
	}

	// 목표를 찾은 뒤에는 최종 경로만 '*'로 애니메이션 출력한다.
	if (isPathAnimating)
	{
		pathTimer.Tick(deltaTime);
		if (pathTimer.IsTimeOut())
		{
			pathTimer.Reset();

			if (pathIndex < static_cast<int>(path.size()))
			{
				Node* const node = path[pathIndex];
				const int originalCell = grid[node->position.y][node->position.x];
				int& cell = displayGrid[node->position.y][node->position.x];
				
				// 위험 지형이면
				if (originalCell == 4)
				{
					cell = 7;
					hasSteppedOnDanger = true;
					isGameOver = true;
					resultState = ResultState::GameOver;
					isPathAnimating = false;
				}
				// S, G, 위험을 제외한 모든 것들
				else if (cell != 2 && cell != 3)
				{
					cell = 6;
				}
				++pathIndex;
			}
			// 경로를 끝까지 다 그린 상태
			else
			{
				isPathAnimating = false;

				// 목표 지점까지 안전하게 도달했으면 다음 스테이지로 넘어간다.
				if (hasPath && !hasSteppedOnDanger)
				{
					if (stageNum < stageCount)
					{
						remainingTime += stageClearTimeBonus;
						resultState = ResultState::StageClear;
						stageClearTimer.Reset();
					}
					else
					{
						isGameOver = true;
						resultState = ResultState::GameClear;
					}
				}
			}
		}
	}
}

void GameLevel::Draw()
{
	if (resultState != ResultState::None)
	{
		if (resultState == ResultState::GameClear)
		{
			Renderer::Get().Submit("Game Clear", Vector2(0, 0), Color::Green);
			Renderer::Get().Submit("All Stage Clear!", Vector2(0, 2), Color::White);
			sprintf_s(timerString, 128, "Time Left : %.1f", remainingTime);
			Renderer::Get().Submit(timerString, Vector2(0, 4), Color::White);
		}
		else if (resultState == ResultState::StageClear)
		{
			sprintf_s(resultString, 128, "Stage %d Clear!", stageNum);
			Renderer::Get().Submit("Stage Clear", Vector2(0, 0), Color::Green);
			Renderer::Get().Submit(resultString, Vector2(0, 2), Color::White);
			Renderer::Get().Submit("Time : +10", Vector2(0, 4), Color::Green);
			Renderer::Get().Submit("Loading Next Stage...", Vector2(0, 6), Color::White);
		}
		else
		{
			sprintf_s(resultString, 128, "Game Over - Stage %d", stageNum);
			Renderer::Get().Submit("Game Over", Vector2(0, 0), Color::Red);
			Renderer::Get().Submit(resultString, Vector2(0, 2), Color::White);
			sprintf_s(timerString, 128, "Time Left : %.1f", remainingTime);
			Renderer::Get().Submit(timerString, Vector2(0, 4), Color::White);
		}

		for (int ix = 0; ix < resultMenuCount && resultState != ResultState::StageClear; ++ix)
		{
			Color textColor = (ix == resultMenuIndex) ? selectedColor : unSelectedColor;
			Renderer::Get().Submit(resultMenuItems[ix], Vector2(0, 7 + ix), textColor);
		}

		return;
	}

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
				color = Color::Blue;
				break;
			case 6:
				text = "* ";
				color = Color::Red | Color::Green;
				break;
			case 7:
				text = "* ";
				color = Color::White | Color::BgRed;
				break;
			case 8:
				text = "# ";
				color = Color::Green;
				break;
			}

			// 두 칸 단위로 찍어서 grid가 찌그러지지 않게 맞춘다.
			Renderer::Get().Submit(text, Vector2(x * 2, y), color);
		}
	}

	if (isGameOver)
	{
		// 결과 화면은 상단 분기에서 따로 그린다.
	}

	ShowUI();
}

void GameLevel::LoadMap(const char* filename)
{
	// 파일 로드.
	// 최종 파일 경로 만들기.
	char filePath[2048] = {};
	sprintf_s(filePath, 2048, "../Assets/%s", filename);

	// 파일 열기.
	FILE* file = nullptr;
	fopen_s(&file, filePath, "rt");

	// 예외 처리.
	if (!file)
	{
		std::cerr << "Error: Failed to open map file.\n";
		__debugbreak();
	}

	// 기존 맵 데이터 초기화.
	grid.clear();
	displayGrid.clear();
	initialGrid.clear();
	startPosition = Vector2::Zero;
	goalPosition = Vector2::Zero;
	initialStartPosition = Vector2::Zero;
	initialGoalPosition = Vector2::Zero;
	isSearching = false;
	isPathAnimating = false;
	hasPathSearched = false;
	hasPath = false;
	hasSteppedOnDanger = false;
	isGameOver = false;
	resultState = ResultState::None;
	resultMenuIndex = 0;
	path.clear();
	pathIndex = 0;
	searchTimer.Reset();
	pathTimer.Reset();
	stageClearTimer.Reset();
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
			8: 외곽 고정 벽
			*/

			// 공백으로 비워둔 맵은 빈 공간으로 처리.
			int cell = 0;
			if ((mapCharacter >= '0' && mapCharacter <= '4') || mapCharacter == '8')
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
	initialGrid = grid;
	displayGrid = grid;
	initialStartPosition = startPosition;
	initialGoalPosition = goalPosition;
	mapHeight = static_cast<int>(displayGrid.size());
	mapWidth = mapHeight > 0 ? static_cast<int>(displayGrid[0].size()) : 0;

	// 파일 닫기.
	fclose(file);
}

void GameLevel::ShowUI()
{
	if (resultState != ResultState::None)
	{
		return;
	}

	sprintf_s(inputString, 128, "Space : Find Path");
	Renderer::Get().Submit(inputString, Vector2(0, mapHeight + 1), Color::White);

	sprintf_s(stageString, 128, "Stage : %d / %d", stageNum, stageCount);
	Renderer::Get().Submit(stageString, Vector2(0, mapHeight + 2), Color::White);

	sprintf_s(timerString, 128, "Time : %.1f", remainingTime);
	Renderer::Get().Submit(timerString, Vector2(0, mapHeight + 3), Color::White);

	if (isSearching)
	{
		sprintf_s(stateString, 128, "Status : Searching...");
	}
	else if (isPathAnimating)
	{
		sprintf_s(stateString, 128, "Status : Drawing Path...");
	}
	else if (resultState == ResultState::StageClear)
	{
		sprintf_s(stateString, 128, "Status : Stage Clear");
	}
	else if (hasSteppedOnDanger)
	{
		sprintf_s(stateString, 128, "Status : GameOver");
	}
	else if (isGameOver && stageNum >= stageCount)
	{
		sprintf_s(stateString, 128, "Status : Stage Clear");
	}
	else if (!hasPathSearched)
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

	Renderer::Get().Submit(stateString, Vector2(0, mapHeight + 4), Color::White);
}

void GameLevel::RetryGame()
{
	stageNum = 1;
	remainingTime = gameTimeLimit;
	resultState = ResultState::None;
	resultMenuIndex = 0;
	stageClearTimer.Reset();
	LoadMap(stageFiles[stageNum - 1]);
}
