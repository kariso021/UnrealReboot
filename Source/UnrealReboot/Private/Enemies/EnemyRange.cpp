#include "Enemies/EnemyRange.h"

AEnemyRange::AEnemyRange()
{
	// Initialization codehere
}

void AEnemyRange::BeginPlay()
{
	Super::BeginPlay();
	// Range-specific initialization or begin play logic
}

void AEnemyRange::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Range-specific tick code
}