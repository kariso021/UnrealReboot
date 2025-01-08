// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "../../Melee/DataOfMelee.h"
#include "BTT_MeleeAttackCPP.generated.h"

/**
 * 
 */
UCLASS()
class UBTT_MeleeAttackCPP : public UBTTask_BlackboardBase
{
	GENERATED_BODY()


protected:

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UFUNCTION()
	void OnAttackFinished();


private:

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector AttackTargetKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector AttackRadiusKey;

	uint8 TokenNeeded;

	EM_Melee_Atttacks AttackName;
	

	UBehaviorTreeComponent* CachedOwnerComp;

	class AEnemyBase* EnemyBase;
	class AAIController* CachedAIController;
	
};
