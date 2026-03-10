#include "Player.h"
#include "Engine/Engine.h"
#include "Core/Input.h"
#include "Level/Level.h"
#include "Actor/PlayerBullet.h"

#include "Interface/ICanMove.h"

Player::Player()
	:super("P", Vector2::Zero, Color::Blue | Color::Red),
	fireMode(FireMode::OneShot)
{
	// 레이어 설정
	sortingOrder = 5;

	// 플레이어 생성 위치 설정.
	int xPosition = (Engine::Get().GetWidth() / 2) - (actorWidth / 2) - 13;
	int yPosition = Engine::Get().GetHeight() -2;
	SetPosition(Vector2(xPosition, yPosition));

	// 타이머 목표 시간 설정.
	timer.SetTargetTime(fireInterval);
}

Player::~Player()
{

}

void Player::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	// 경과 시간 업데이트.
	timer.Tick(deltaTime);

	// 인터페이스 확인.
	static ICanMove* canPlayerMoveInterface = nullptr;

	// 오너쉽 확인
	if (!canPlayerMoveInterface && GetOwner())
	{
		// 인더페이스로 형변환.
		canPlayerMoveInterface = dynamic_cast<ICanMove*>(GetOwner());
	}

	// 상하 방향키 입력 처리.
	if (Input::Get().GetKey('W')) // W키
	{
		MoveUp(canPlayerMoveInterface);
	}
	if (Input::Get().GetKey('S')) // S키
	{
		MoveDown(canPlayerMoveInterface);
	}

	// 좌우 방향키 입력처리.
	if (Input::Get().GetKey('A')) // A키
	{
		MoveLeft(canPlayerMoveInterface);
	}
	if (Input::Get().GetKey('D')) // D키
	{
		MoveRight(canPlayerMoveInterface);
	}

	// 마우스 사격.
	if (fireMode == FireMode::OneShot)
	{
		if (Input::Get().GetMouseButtonDown(0))
		{
			Fire();
		}
	}
	else if (fireMode == FireMode::Repeat)
	{
		if (Input::Get().GetMouseButton(0))
		{
			FireInterval();
		}
	}
	
	// 발사 모드 전환.
	if (Input::Get().GetKeyDown('R'))
	{
		int mode = static_cast<int>(fireMode);
		mode = 1 - mode;
		fireMode = static_cast<FireMode>(mode);
	}

	
}

void Player::MoveUp(ICanMove* canMove)
{
	// 이동가능 확인 -> 벽도 확인
	Vector2 newPos(GetPosition().x, GetPosition().y - 1);
	if (canMove->CanMove(*this, newPos))
	{
		// 위로 이동.
		position.y -= 1;
	}

	// 콘솔 높이 좌표 검사.
	if (position.y < 0)
	{
		position.y = 0;
	}
}

void Player::MoveDown(ICanMove* canMove)
{
	// 이동가능 확인 -> 벽도 확인
	Vector2 newPos(GetPosition().x, GetPosition().y + 1);
	if (canMove->CanMove(*this, newPos))
	{
		// 아래로 이동.
		position.y += 1;
	}

	// 콘솔 높이 좌표 검사.
	if (position.y + actorHeight > Engine::Get().GetHeight())
	{
		position.y += 1;
	}
}

void Player::MoveRight(ICanMove* canMove)
{
	// 이동가능 확인 -> 벽도 확인
	Vector2 newPos(GetPosition().x + 1, GetPosition().y);
	if (canMove->CanMove(*this, newPos))
	{
		// 오른쪽 이동.
		position.x += 1;
	}
	

	// 콘솔 너비 좌표 검사.
	if (position.x + actorWidth > Engine::Get().GetWidth())
	{
		position.x -= 1;
	}
}

void Player::MoveLeft(ICanMove* canMove)
{

	// 이동가능 확인 -> 벽도 확인
	Vector2 newPos(GetPosition().x - 1, GetPosition().y);
	if (canMove->CanMove(*this, newPos))
	{
		// 왼쪽 이동 처리.
		position.x -= 1;
	}

	// 좌표 검사.
	if (position.x < 0)
	{
		position.x = 0;
	}
}

void Player::Fire()
{
	// 경과 시간 초기화.
	//elapsedTime = 0.0f;
	timer.Reset();

	// 위치 설정.
	Vector2 bulletPosition(
		position.x + (actorWidth / 2),
		position.y
	);

	// 액터 생성.
	GetOwner()->AddNewActor(new PlayerBullet(bulletPosition));
}

void Player::FireInterval()
{
	// 발사 가능 여부 확인.
	if (!CanShoot())
	{
		return;
	}

	// 발사.
	Fire();
}

bool Player::CanShoot() const
{
	// 경과 시간 확인.
	// 발사 간격보다 더 많이 흘렀는지.
	return timer.IsTimeOut();
}
