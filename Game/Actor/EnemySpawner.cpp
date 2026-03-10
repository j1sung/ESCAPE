#include "EnemySpawner.h"
#include "Level/Level.h"
#include "Util/Util.h"
#include "Actor/Enemy.h"

// 적 생성 시 사용할 글자 값.
static const char* enemyType[] =
{
	"x",
	"X",
	"x+x",
};

EnemySpawner::EnemySpawner(const Vector2& position, const int rightX)
	:super("", position), rightX(rightX)
{
}

EnemySpawner::~EnemySpawner()
{
}

void EnemySpawner::BeginPlay()
{
	super::BeginPlay();
	SpawnEnemy(GetPosition(), rightX);
}

void EnemySpawner::SpawnEnemy(const Vector2& position, const int rightX)
{
	// 적 종류 개수 파악
	int length = sizeof(enemyType) / sizeof(enemyType[0]);

	for (int ix = 0; ix < Util::Random(1, maxEnemy); ++ix)
	{
		// 랜덤 인덱스.
		int index = Util::Random(0, length - 1);

		// 적 생성 요청.
		GetOwner()->AddNewActor(new Enemy(enemyType[index], position, rightX));
	}
}
