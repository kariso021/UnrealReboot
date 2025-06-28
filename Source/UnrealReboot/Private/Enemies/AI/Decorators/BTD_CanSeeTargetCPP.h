// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTD_CanSeeTargetCPP.generated.h"

/**
 * 
 */
UCLASS()
class UBTD_CanSeeTargetCPP : public UBTDecorator
{
	GENERATED_BODY()

protected:
    virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;


private:
    /** ���� ��� Ű */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default", meta = (AllowPrivateAccess = "true", DisplayName = "Attack Target Key"))
    FBlackboardKeySelector AttackTargetKey;

    /** AI �þ� ä�� */
    UPROPERTY(EditAnywhere, Category = "LineTrace")
    TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;
	
};
