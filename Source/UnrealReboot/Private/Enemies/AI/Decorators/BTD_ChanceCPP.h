// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTD_ChanceCPP.generated.h"

/**
 * 
 */
UCLASS()
class UBTD_ChanceCPP : public UBTDecorator
{
	GENERATED_BODY()
	

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

private:

	float Percent;
};
