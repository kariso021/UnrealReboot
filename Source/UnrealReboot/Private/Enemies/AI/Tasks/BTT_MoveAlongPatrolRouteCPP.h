// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_MoveAlongPatrolRouteCPP.generated.h"

/**
 * 
 */
UCLASS()
class UBTT_MoveAlongPatrolRouteCPP : public UBTTaskNode
{
	GENERATED_BODY()


protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	/** Called when task is aborted */
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;



private:
	
};
