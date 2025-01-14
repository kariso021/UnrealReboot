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
	// Movement Speed 설정 (Enum 사용)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI")
	float SetMovementSpeed(EM_MovementSpeed Speed);

	//// 이상적인 Range 가져오기
	//UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI")
	//FRangeofState GetIdealRange();



	// 이상적인 Range 가져오기
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI")
	float GetAttackRadius();

	// 이상적인 Range 가져오기
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI")
	float GetDefendRadius();



	// 무기 장착
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI")
	void EquipWeapon();

	// 무기 해제
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI")
	void UnequipWeapon();

	// 공격 수행
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI")
	void Attack(AActor* AttackTarget);

	// 지정된 위치로 점프
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI")
	void JumpToDestination(FVector Destination);

	// 공격 시작
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI")
	bool AttackStart(AActor* AttackTarget, int TokenNeeded);

	// 공격 종료
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI")
	void AttackEnd(AActor* AttackTarget);

	// 공격 토큰 저장
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI")
	void StoreAttackToken(AActor* AttackTarget, int Amount);

	// 순찰 경로 가져오기
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI")
	APatrolRoute* GetPatrolRoute();
};