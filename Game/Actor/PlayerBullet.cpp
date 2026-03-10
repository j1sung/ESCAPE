#include "PlayerBullet.h"
#include "Core/Input.h"
#include "math.h"


PlayerBullet::PlayerBullet(const Vector2& position)
	:super("@", position, Color::Blue)
{
	xPos = static_cast<float>(position.x);
	yPos = static_cast<float>(position.y);
}

PlayerBullet::~PlayerBullet()
{
}

void PlayerBullet::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	if (!VelocityInitialized)
	{
		// 마우스 좌표로 이동 처리.
		Vector2 target = Input::Get().MousePosition();
		float dx = static_cast<float>(target.x - position.x);
		float dy = static_cast<float>(target.y - position.y);

		float len = sqrtf(dx * dx + dy * dy);
		if (len > 0.0001f)
		{
			float nx = dx / len;
			float ny = dy / len;
			vx = nx * moveSpeed;
			vy = ny * moveSpeed;
		}
		VelocityInitialized = true;
	}

	xPos += vx * deltaTime;
	yPos += vy * deltaTime;

	SetPosition(Vector2(static_cast<int>(xPos), static_cast<int>(yPos)));
}
