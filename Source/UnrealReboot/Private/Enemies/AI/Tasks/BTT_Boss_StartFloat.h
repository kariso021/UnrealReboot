// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "../../Bose/EnemyBose.h"
#include "BTT_Boss_StartFloat.generated.h"

/**
 * 
 */
UCLASS()
class UBTT_Boss_StartFloat : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTT_Boss_StartFloat();

	protected:
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
