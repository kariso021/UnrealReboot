// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTS_UpdateDistanceToTargetCPP.generated.h"

/**
 * 
 */
UCLASS()
class UBTS_UpdateDistanceToTargetCPP : public UBTService
{
	GENERATED_BODY()

protected:
    /** ���� ������Ʈ */
    virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;


private:
    /** ���� ��� Ű */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default", meta = (AllowPrivateAccess = "true", DisplayName = "Attack Target Key"))
    FBlackboardKeySelector AttackTargetKey;

    /** ��ǥ���� �Ÿ� ���� Ű */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default", meta = (AllowPrivateAccess = "true", DisplayName = "Distance To Target Key"))
    FBlackboardKeySelector DistanceToTargetKey;

	
};
