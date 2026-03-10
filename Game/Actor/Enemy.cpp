#include "Enemy.h"
#include "Level/Level.h"
#include "Level/GameLevel.h"
#include "Util/Util.h"
#include "Actor/EnemyBullet.h"
#include "Actor/EnemyDestroyEffect.h"

#include "Interface/ICanMove.h"

static int GetMaxLineWidth(const char* image)
{
	int maxLine = 0;
	const char* lineStart = image;

	while (lineStart)
	{
		const char* lineEnd = strchr(lineStart, '\n');
		int lineLength = lineEnd
			? static_cast<int>(lineEnd - lineStart)
			: static_cast<int>(strlen(lineStart));

		if (lineLength > maxLine) maxLine = lineLength;

		if (!lineEnd) break;
		lineStart = lineEnd + 1;
	}

	return maxLine;
}

Enemy::Enemy(const char* image, const Vector2& position, const int rightX)
	:super(image, position, Color::Red | Color::BgWhite)
{
	// 레이어 설정
	sortingOrder = 3;
	
	// 랜덤 (오른쪽 또는 왼쪽으로 이동할지 결정).
	int random = Util::Random(0, 1);

	// Todo: 임시로 적 생성 좌표 관련 코드들 싹 정리하자!
	// (임시) Actor의 actorWidth대체: 적의 우측끝 생성 보정을 위해 -> 적 너비 구하기
	const int lineWidth = GetMaxLineWidth(image);

	// (임시) 적 랜덤 y좌표 생성할때 겹치면 안되니까 일단 중복되면 다시 뽑게 함.
	static int lastY = -1;

	if (random == 0)
	{
		// 화면 오른쪽에서 생성.
		direction = MoveDirection::Left;
		xPosition = static_cast<float>(rightX - lineWidth + 1);
	}
	else
	{
		// 화면 왼쪽에서 생성.
		direction = MoveDirection::Right;
		xPosition = static_cast<float>(position.x);
	}
	
	// (임시) 적 y좌표 임시로 랜덤에서 중복제거 루프 돌린거라 2명까지만 됨. 3명부턴 오류여지 있음.
	int yPosition = Util::Random(position.y, position.y + 2);
	while (yPosition == lastY)
	{
		yPosition = Util::Random(position.y, position.y + 2);
	}
	lastY = yPosition;

	// 이동 방향에 따른 적 위치 설정.
	SetPosition(Vector2(static_cast<int>(xPosition), static_cast<int>(yPosition)));

	// 발사 타이머 목표 시간 설정.
	timer.SetTargetTime(Util::RandomRange(1.0f, 3.0f));
}

Enemy::~Enemy()
{
}

void Enemy::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	moveTimer += deltaTime;
	if (moveTimer < moveInterval)
	{
		return;
	}
	moveTimer = 0.0f;

	// 좌표 검사.
	// 다음 좌표 이동 가능한지 검사
	//static ICanMove* canMoveInterface = nullptr;

	// 오너쉽 확인(null check)
	//if (!canMoveInterface && GetOwner())
	//{
	//	// 인터페이스로 형변환
	//		//}
	
	// 이동 방향
	float dir = direction == MoveDirection::Left ? -1.0f : 1.0f;

	// 다음 위치 계산
	Vector2 newPosition = GetPosition();
	newPosition.x += (dir < 0) ? -1 : 1;

	// 이동 가능 여부 판단.
	//if (canMoveInterface->CanMove(*this, newPosition))
	GameLevel* level = GetOwner()->As<GameLevel>();
	if(level->IsInWhiteSpaceAt(*this, newPosition))
	{
		//newPosition.x += moveSpeed * dir * deltaTime;
		SetPosition(newPosition);
	}
	else
	{
		// 방향 반전
		direction = static_cast<MoveDirection>(1 - (int)direction);
	}

	
}

void Enemy::OnDamaged()
{
	// 액터 제거.
	Destroy();

	// 이펙트 생성 (재생을 위해).
	GetOwner()->AddNewActor(new EnemyDestroyEffect(position));
}
