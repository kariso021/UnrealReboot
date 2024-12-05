// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/EnemyAIInterface.h"

// Add default functionality here for any IEnemyAIInterface functions that are not pure virtual.

float IEnemyAIInterface::SetMovementSpeed(EM_MovementSpeed Speed)
{
	return 0.0f;
}

FRangeofState IEnemyAIInterface::GetIdealRange()
{
	return FRangeofState();
}

void IEnemyAIInterface::EquipWeapon()
{
}

void IEnemyAIInterface::UnequipWeapon()
{
}

void IEnemyAIInterface::Attack(AActor* AttackTarget)
{
}

void IEnemyAIInterface::JumpToDestination(FVector Destination)
{
}

bool IEnemyAIInterface::AttackStart(AActor* AttackTarget, int TokenNeeded)
{
	return false;
}

void IEnemyAIInterface::AttackEnd(AActor* AttackTarget)
{
}

void IEnemyAIInterface::StoreAttackToken(AActor* AttackTarget, int Amount)
{
}

APatrolRoute* IEnemyAIInterface::GetPatrolRoute()
{
	return nullptr;
}
