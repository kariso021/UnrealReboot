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
    /** AI�� ���� Ȯ�� */
    virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

    /** �̻����� �Ÿ� Ű */
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector IdealRangeKey;

    /** ��ǥ Ű */
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector TargetKey;

    /** ���� ���� */
    UPROPERTY(EditAnywhere, Category = "Condition")
    float ErrorMargin = 50.0f;
	
};
