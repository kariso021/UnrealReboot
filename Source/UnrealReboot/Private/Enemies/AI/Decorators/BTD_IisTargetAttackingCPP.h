// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTD_IisTargetAttackingCPP.generated.h"

/**
 * 
 */
UCLASS()
class UBTD_IisTargetAttackingCPP : public UBTDecorator
{
	GENERATED_BODY()

protected:

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;


private:
	// Blackboard에서 대상 Actor를 찾기 위한 Key
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default", meta = (AllowPrivateAccess = "true", DisplayName = "Attack Target Key"))
	FBlackboardKeySelector AttackTargetKey;
	
};
