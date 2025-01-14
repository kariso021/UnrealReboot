// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_MeleeBlockCPP.generated.h"

/**
 * 
 */
UCLASS()
class UBTT_MeleeBlockCPP : public UBTTaskNode
{
	GENERATED_BODY()

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UFUNCTION()
	void OnBlockFinished();


protected:

	class AAIController* CachedAIController;
	UBehaviorTreeComponent* CachedOwnerComp;

	class AEnemyMelee* EnemyBase;
	
};
