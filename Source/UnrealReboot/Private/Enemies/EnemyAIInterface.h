// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PatrolRoute.h"
#include "DataOfEnemies.h"
#include "EnemyAIInterface.generated.h"

class AEnemyBase;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UEnemyAIInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class IEnemyAIInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION()
	virtual float SetMovementSpeed(EM_MovementSpeed Speed);//Enum이 들어가야함 Return 으로 Speed 받으면 됨

	UFUNCTION()
	virtual FRangeofState GetIdealRange();//두가지 종류의 Range 가 필요함 Return 의 형태로 FRangeofState

	UFUNCTION()
	virtual void EquipWeapon ();

	UFUNCTION()
	virtual void UnequipWeapon();

	UFUNCTION()
	virtual void Attack(AActor* AttackTarget);

	UFUNCTION()
	virtual void JumpToDestination(FVector Destination);

	UFUNCTION()
	virtual bool AttackStart(AActor* AttackTarget,int TokenNeeded);

	UFUNCTION()
	virtual void AttackEnd(AActor* AttackTarget);

	UFUNCTION()
	virtual void StoreAttackToken(AActor* AttackTarget,int Amount);

	UFUNCTION()
	virtual APatrolRoute* GetPatrolRoute();



};
