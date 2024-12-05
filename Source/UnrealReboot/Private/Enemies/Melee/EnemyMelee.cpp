#include "Enemies/Melee/EnemyMelee.h"

AEnemyMelee::AEnemyMelee()
{
	// Initialization codehere
}

void AEnemyMelee::BeginPlay()
{
	Super::BeginPlay();
	// melee-specific initialization or begin play logic
}

void AEnemyMelee::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// melee-specific tick code
}