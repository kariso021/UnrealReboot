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

public:
	// Movement Speed ���� (Enum ���)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI")
	float SetMovementSpeed(EM_MovementSpeed Speed);

	//// �̻����� Range ��������
	//UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI")
	//FRangeofState GetIdealRange();



	// �̻����� Range ��������
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI")
	float GetAttackRadius();

	// �̻����� Range ��������
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI")
	float GetDefendRadius();



	// ���� ����
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI")
	void EquipWeapon();

	// ���� ����
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI")
	void UnequipWeapon();

	// ���� ����
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI")
	void Attack(AActor* AttackTarget);

	// ������ ��ġ�� ����
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI")
	void JumpToDestination(FVector Destination);

	// ���� ����
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI")
	bool AttackStart(AActor* AttackTarget, int TokenNeeded);

	// ���� ����
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI")
	void AttackEnd(AActor* AttackTarget);

	// ���� ��ū ����
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI")
	void StoreAttackToken(AActor* AttackTarget, int Amount);

	// ���� ��� ��������
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI")
	APatrolRoute* GetPatrolRoute();
};