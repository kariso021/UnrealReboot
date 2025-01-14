// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "BTD_IsWithinIdealRangeCPP.generated.h"

/**
 * 
 */
UCLASS()
class UBTD_IsWithinIdealRangeCPP : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()


protected:
    /** AI의 조건 확인 */
    virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

    /** 이상적인 거리 키 */
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector IdealRangeKey;

    /** 목표 키 */
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector TargetKey;

    /** 에러 마진 */
    UPROPERTY(EditAnywhere, Category = "Condition")
    float ErrorMargin = 50.0f;
	
};
