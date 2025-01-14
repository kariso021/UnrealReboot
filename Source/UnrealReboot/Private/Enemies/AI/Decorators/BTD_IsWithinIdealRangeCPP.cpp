// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/AI/Decorators/BTD_IsWithinIdealRangeCPP.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Actor.h"


bool UBTD_IsWithinIdealRangeCPP::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    // 블랙보드 컴포넌트 가져오기
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComp)
    {
        return false;
    }

    // 목표 가져오기
    AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetKey.SelectedKeyName));
    if (!TargetActor)
    {
        return false;
    }

    // 이상적인 거리 가져오기
    float IdealRange = BlackboardComp->GetValueAsFloat(IdealRangeKey.SelectedKeyName);
    float DistanceToTarget = OwnerComp.GetAIOwner()->GetPawn()->GetDistanceTo(TargetActor);

    // 거리와 에러 마진 비교
    return FMath::Abs(DistanceToTarget - IdealRange) <= ErrorMargin;
}
