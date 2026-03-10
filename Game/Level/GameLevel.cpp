#include "Level/GameLevel.h"
#include "Render/Renderer.h"
#include "Actor/DieMenu.h"
#include "Actor/Player.h"
#include "Actor/White.h"
#include "Actor/Enemy.h"
#include "Actor/PlayerBullet.h"
#include "Actor/Wall.h"
#include "Engine/Engine.h"

#include <iostream>

static bool ContainsAABB(const Actor& space, const Actor& moving, const Vector2& pos)
{
	const int mXMin = pos.x;
	const int mXMax = pos.x + moving.GetWidth() - 1;
	const int mYMin = pos.y;
	const int mYMax = pos.y + moving.GetHeight() - 1;

	const Vector2 sPos = space.GetPosition();
	const int sXMin = sPos.x;
	const int sXMax = sPos.x + space.GetWidth() - 1;
	const int sYMin = sPos.y;
	const int sYMax = sPos.y + space.GetHeight() - 1;

	return (mXMin >= sXMin && mXMax <= sXMax &&
		mYMin >= sYMin && mYMax <= sYMax);
}

/*
0: 바닥(Ground)
p: 플레이어(Player)
e: 적(Enemy)
*/

GameLevel::GameLevel()
{
	// 임시로 첫 맵 세팅 바로
	// Todo: 이거 맵 클리어 이후 벽을 나가면 다음 맵이나 다시 맵 로드하게끔 해야함.
	LoadMap("Stage1.txt");
	AddNewActor(new Player());
}

GameLevel::~GameLevel()
{
}

// (임시 검사)직접 Actor넣어서 White 질의하는 충돌처리 함수
bool GameLevel::IsInWhiteSpaceAt(const Actor& actor, const Vector2& newPosition) const
{
	for (Actor* const other : actors)
	{
		if (!other->IsTypeOf<White>())
			continue;
		if (ContainsAABB(*other, actor, newPosition))
			return true;
	}
	return false;
}

void GameLevel::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	// 충돌 판정 처리.
	ProcessCollisionPlayerAttackAndEnemy();
	ProcessCollisionPlayerAndEnemyIsAlive();


	// 남은 적 확인
	bool enemyLeft = false;
	for (Actor* const actor : actors)
	{
		if (actor->IsTypeOf<Enemy>())
		{
			enemyLeft = true;
			hadEnemy = true;
			break;
		}
	}
	if (hadEnemy && !enemyLeft)
	{
		isStageCleared = true;
		ProcessCollisionPlayerAndWall();
	}
}

void GameLevel::Draw()
{
	super::Draw();

	if (isPlayerDead && !deadSubmitted)
	{
		// 최초 죽음 메시지 출력
		Renderer::Get().Submit("!Dead!", playerDeadPosition, Color::Red);

		// UI 보여주기.
		ShowUI();

		// 화면에 바로 표시.
		Renderer::Get().Draw();

		// 프로그램 정지.
		Sleep(2000);

		deadSubmitted = true;

		// Die 시 선택 UI 띄우기.
		//AddNewActor(new DieMenu(data));
	}

	// UI 보여주기.
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

	// 맵 읽기.
	// 맵 크기 파악: FP(File Position) 포인터를 파일의 끝으로 이동.
	fseek(file, 0, SEEK_END);

	// 파일 크기 파악(끝지점으로 간 FP를 읽음)
	size_t fileSize = ftell(file);

	// FP 처음으로 되돌리기.
	rewind(file);

	// 파일에서 데이터를 읽어올 버퍼 생성.
	char* data = new char[fileSize + 1];

	// 데이터 읽기.
	size_t readSize = fread(data, sizeof(char), fileSize, file);

	// 읽어온 문자열 분석(파싱)해서 출력.
	// 인덱스로 한문자씩 읽기.
	int index = 0;

	// 객체를 생성할 위치 값.
	Escape::Vector2 position;

	while (true)
	{
		// 종료 조건.
		if (index >= fileSize)
		{
			break;
		}

		// 캐릭터 읽기.
		char mapCharacter = data[index];
		++index;

		// 개행 문자 처리.
		if (mapCharacter == '\n')
		{
			// y좌표는 하나 늘리고, x좌표 초기화.
			++position.y;
			position.x = 0;
			continue;
		}

		/*
		#, 1: 벽(Wall)
		0: 블랙 스페이스(Black space)
		2: 화이트 스페이스(White space) -> 적 스포너 가짐.
		p: 플레이어(Player)
		*/


		// 한문자씩 처리.
		switch (mapCharacter)
		{
		case'2':
			AddNewActor(new Wall(position));
			break;
		case'0':
			//AddNewActor(new Black(position));
			break;
		case'1':
			AddNewActor(new White(position));
			break;
		//case'p': // 일단 임시로 플레이어 시작 위치는 고정
		//	AddNewActor(new Player()); 
			//AddNewActor(new Black(position));
			break;
		}

		// x좌표 증가 처리.
		++position.x;
	}
	
	// 사용한 버퍼 해제.
	delete[] data;

	// 파일 닫기.
	fclose(file);
}

bool GameLevel::CanMove(Actor& mover, const Vector2& nextPos)
{
	// 현재 액터의 좌표.
	// Vector2 currentPos = mover.GetPosition();

	// 적 액터였을 경우 -> 화이트 스페이스는 이미지만 키워서 좌표로 검사를 할 수 가 없다. -> 콜라이전 검사를 해야함!
	//if (mover.IsTypeOf<Enemy>())
	//{
	//	for (Actor* const actor : actors)
	//	{
	//		if (actor->GetPosition() == nextPos)
	//		{
	//			// 액터가 화이트 스페이스인지 확인.
	//			if (actor->IsTypeOf<White>())
	//			{
	//				return true;
	//			}
	//			return false;
	//		}
	//	}
	//}
	// 플레이어 액터였을 경우
	if (mover.IsTypeOf<Player>())
	{
		for (Actor* const actor : actors)
		{
			if (actor->GetPosition() == nextPos)
			{
				// 액터가 화이트 스페이스인지 확인.
				if (actor->IsTypeOf<Wall>())
				{
					if (isStageCleared)
					{
						return true;
					}
					return false;
				}
			}
		}
	}

	// 기본적으로 벽이 없다면 true.
	return true;
}

// [플레이어 공격]: 플레이어 발사체-> 적 body
void GameLevel::ProcessCollisionPlayerAttackAndEnemy()
{
	// 플레이어 탄약과 적 액터 필터링.
	std::vector<Actor*> bullets;
	std::vector<Enemy*> enemies;

	// 액터 필터링.
	for (Actor* const actor : actors)
	{
		if (actor->IsTypeOf<PlayerBullet>())
		{
			bullets.emplace_back(actor);
			continue;
		}

		if (actor->IsTypeOf<Enemy>())
		{
			enemies.emplace_back(actor->As<Enemy>());
		}
	}

	// 판정 안해도 되는지 확인.
	if (bullets.size() == 0 || enemies.size() == 0)
	{
		return;
	}

	// 충돌 판정.
	for (Actor* const bullet : bullets)
	{
		for (Enemy* const enemy : enemies)
		{
			// AABB 겹침 판정.
			if (bullet->TestIntersect(enemy))
			{
				enemy->OnDamaged();
				bullet->Destroy();

				// 점수 추가.
				LevelManager::Get().GetGameData().score += 1;
				continue;
			}
		}
	}
}

// [플레이어 수집]: 플레이어 body -> 적 body(죽었을때만)
void GameLevel::ProcessCollisionPlayerAndEnemyIsDead()
{
}

// [플레이어 스테이지 클리어]: 플레이어 body -> 벽 body(클리어시만)
void GameLevel::ProcessCollisionPlayerAndWall()
{
	if (isStageCleared)
	{
		// 액터 필터링 변수
		std::vector<Actor*> player;
		std::vector<Wall*> walls;

		// 액터 필터링
		for (Actor* const actor : actors)
		{
			if (actor->IsTypeOf<Player>())
			{
				player.emplace_back(actor);
				continue;
			}
			if (actor->IsTypeOf<Wall>())
			{
				walls.emplace_back(actor->As<Wall>());
			}
		}
		// 판정 안해도 되는지 확인.
		if (player.size() == 0 || walls.size() == 0)
		{
			return;
		}

		// 충돌 판정.
		for (Actor* const p : player)
		{
			for (Wall* const wall : walls)
			{
				// AABB 겹침 판정.
				if (p->TestIntersect(wall))
				{
					Renderer::Get().Submit("Game Clear!", Vector2(Engine::Get().GetWidth() - 30, Engine::Get().GetHeight() - 25));
					Sleep(1000);
					LevelManager::Get().SetState(State::Menu);
				}
			}
		}
	}
}

// [적 공격]: 적 body(살아있을때만) -> 플레이어 body - (타격후 3초간 무적)
void GameLevel::ProcessCollisionPlayerAndEnemyIsAlive()
{
}

// [적 이동]: 화이트 스페이스 범위에서만 이동 가능
void GameLevel::ProcessCollisionEnemyAndWhieSpace()
{
	// 액터 필터링 변수
	std::vector<Actor*> whites;
	std::vector<Enemy*> enemies;

	// 액터 필터링
	for (Actor* const actor : actors)
	{
		if (actor->IsTypeOf<White>())
		{
			whites.emplace_back(actor);
			continue;
		}
		if (actor->IsTypeOf<Enemy>())
		{
			enemies.emplace_back(actor->As<Enemy>());
		}
	}

	// 판정 안해도 되는지 확인.
	if (whites.size() == 0 || enemies.size() == 0)
	{
		return;
	}

	// 충돌 판정.
	for (Actor* const white : whites)
	{
		for (Enemy* const enemy : enemies)
		{
			// AABB 겹침 판정.
			if (white->TestIntersect(enemy))
			{
				
			}
		}
	}
}

void GameLevel::ShowUI()
{
	GameData& data = LevelManager::Get().GetGameData();
	sprintf_s(scoreString, 128, "Score: %d", data.score);
	Renderer::Get().Submit(scoreString, Vector2(Engine::Get().GetWidth()-20, Engine::Get().GetHeight() -20));
}
