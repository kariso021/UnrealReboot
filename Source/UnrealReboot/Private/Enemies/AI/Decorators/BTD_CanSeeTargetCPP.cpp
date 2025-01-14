// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/AI/Decorators/BTD_CanSeeTargetCPP.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Actor.h"
#include "AIController.h"
#include "DrawDebugHelpers.h"

bool UBTD_CanSeeTargetCPP::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    // ������ ������Ʈ ��������
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComp)
    {
        return false;
    }

    // ��ǥ ��������
    AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(AttackTargetKey.SelectedKeyName));
    APawn* ControlledPawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;

    if (!TargetActor || !ControlledPawn)
    {
        return false;
    }

    // ��ǥ�� AI�� ��ġ ��������
    FVector StartLocation = ControlledPawn->GetActorLocation();
    FVector EndLocation = TargetActor->GetActorLocation();

    // �浹 ��� ����
    FHitResult HitResult;

    // ���� Ʈ���̽� ����
    bool bHit = GetWorld()->LineTraceSingleByChannel(
        HitResult,
        StartLocation,
        EndLocation,
        TraceChannel,
        FCollisionQueryParams(FName(TEXT("LineTrace")), false, ControlledPawn)
    );

    // ����� �ð�ȭ
#if WITH_EDITOR
    DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Green, false, 1.0f);
#endif

    // ��ǥ�� ���� Ʈ���̽��� �浹ü�� ������ Ȯ��
    return bHit && HitResult.GetActor() == TargetActor;
}
