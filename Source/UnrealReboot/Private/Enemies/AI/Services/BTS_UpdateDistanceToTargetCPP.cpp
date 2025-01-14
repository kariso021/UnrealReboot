// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/AI/Services/BTS_UpdateDistanceToTargetCPP.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "GameFramework/Actor.h"

void UBTS_UpdateDistanceToTargetCPP::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    // 블랙보드 컴포넌트 가져오기
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComp)
    {
        return;
    }

    // 목표 액터 가져오기
    AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(AttackTargetKey.SelectedKeyName));
    AAIController* AIController = OwnerComp.GetAIOwner();


    //Null로 지정했을때 끊기진 않을까? 여기 유심히 봐야됨 ***
    APawn* ControlledPawn = nullptr;

    if (AIController)
    {
        ControlledPawn = AIController->GetPawn();
    }

    if (!TargetActor || !ControlledPawn)
    {
        return;
    }

    // 목표와의 거리 계산
    float Distance = FVector::Dist(TargetActor->GetActorLocation(), ControlledPawn->GetActorLocation());

    // 계산된 거리를 블랙보드에 저장
    BlackboardComp->SetValueAsFloat(DistanceToTargetKey.SelectedKeyName, Distance);
}
