// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/AI/Services/BTS_UpdateDistanceToTargetCPP.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "GameFramework/Actor.h"

void UBTS_UpdateDistanceToTargetCPP::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    // ������ ������Ʈ ��������
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComp)
    {
        return;
    }

    // ��ǥ ���� ��������
    AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(AttackTargetKey.SelectedKeyName));
    AAIController* AIController = OwnerComp.GetAIOwner();


    //Null�� ���������� ������ ������? ���� ������ ���ߵ� ***
    APawn* ControlledPawn = nullptr;

    if (AIController)
    {
        ControlledPawn = AIController->GetPawn();
    }

    if (!TargetActor || !ControlledPawn)
    {
        return;
    }

    // ��ǥ���� �Ÿ� ���
    float Distance = FVector::Dist(TargetActor->GetActorLocation(), ControlledPawn->GetActorLocation());

    // ���� �Ÿ��� �����忡 ����
    BlackboardComp->SetValueAsFloat(DistanceToTargetKey.SelectedKeyName, Distance);
}
