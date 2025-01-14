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
    /** 서비스 업데이트 */
    virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;


private:
    /** 공격 대상 키 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default", meta = (AllowPrivateAccess = "true", DisplayName = "Attack Target Key"))
    FBlackboardKeySelector AttackTargetKey;

    /** 목표와의 거리 저장 키 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default", meta = (AllowPrivateAccess = "true", DisplayName = "Distance To Target Key"))
    FBlackboardKeySelector DistanceToTargetKey;

	
};
