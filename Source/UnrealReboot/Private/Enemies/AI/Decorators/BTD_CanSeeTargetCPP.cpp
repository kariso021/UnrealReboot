// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/AI/Decorators/BTD_CanSeeTargetCPP.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Actor.h"
#include "AIController.h"
#include "DrawDebugHelpers.h"

bool UBTD_CanSeeTargetCPP::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    // 블랙보드 컴포넌트 가져오기
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComp)
    {
        return false;
    }

    // 목표 가져오기
    AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(AttackTargetKey.SelectedKeyName));
    APawn* ControlledPawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;

    if (!TargetActor || !ControlledPawn)
    {
        return false;
    }

    // 목표와 AI의 위치 가져오기
    FVector StartLocation = ControlledPawn->GetActorLocation();
    FVector EndLocation = TargetActor->GetActorLocation();

    // 충돌 결과 저장
    FHitResult HitResult;

    // 라인 트레이스 실행
    bool bHit = GetWorld()->LineTraceSingleByChannel(
        HitResult,
        StartLocation,
        EndLocation,
        TraceChannel,
        FCollisionQueryParams(FName(TEXT("LineTrace")), false, ControlledPawn)
    );

    // 디버그 시각화
#if WITH_EDITOR
    DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Green, false, 1.0f);
#endif

    // 목표가 라인 트레이스의 충돌체와 같은지 확인
    return bHit && HitResult.GetActor() == TargetActor;
}
