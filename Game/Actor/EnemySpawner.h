#pragma once

#include "Actor/Actor.h"

using namespace Escape;

class EnemySpawner: public Actor
{
	RTTI_DECLARATIONS(EnemySpawner, Actor)

public: 
	EnemySpawner(const Vector2& position, const int rightX);
	~EnemySpawner();

	virtual void BeginPlay() override;

private:
	void SpawnEnemy(const Vector2& position, const int rightX);
	
private:
	int rightX = 0;
	const int maxEnemy = 2;
};

