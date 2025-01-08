// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_ClearFocusCPP.generated.h"

/**
 * 
 */
UCLASS()
class UBTT_ClearFocusCPP : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTT_ClearFocusCPP();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
