// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_MoveToIdealRangeCPP.generated.h"

/**
 * 
 */
UCLASS()
class UBTT_MoveToIdealRangeCPP : public UBTTaskNode
{
	GENERATED_BODY()



protected:


	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;



private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default", meta = (AllowPrivateAccess = "true", DisplayName = "Attack Target Key"))
	FBlackboardKeySelector AttackTargetKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default", meta = (AllowPrivateAccess = "true", DisplayName = "Ideal Range Key"))
	FBlackboardKeySelector IdealRangeKey;
	
};
