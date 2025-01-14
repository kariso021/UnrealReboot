// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_SetIsInvincibleCPP.generated.h"

/**
 * 
 */
UCLASS()
class UBTT_SetIsInvincibleCPP : public UBTTaskNode
{
	GENERATED_BODY()


public:
	UPROPERTY(EditAnywhere, Category = "Value")
	bool Value = false;

protected:

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	class AAIController* CachedAIController;

	
};
