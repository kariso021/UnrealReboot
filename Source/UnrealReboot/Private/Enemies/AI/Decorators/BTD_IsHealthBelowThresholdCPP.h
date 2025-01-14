// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTD_IsHealthBelowThresholdCPP.generated.h"

/**
 * 
 */
UCLASS()
class UBTD_IsHealthBelowThresholdCPP : public UBTDecorator
{
	GENERATED_BODY()

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

	UPROPERTY(EditAnywhere, Category = "AI")
	float HealthThreshold = 50.0f; // ±âº»°ª
	
};
