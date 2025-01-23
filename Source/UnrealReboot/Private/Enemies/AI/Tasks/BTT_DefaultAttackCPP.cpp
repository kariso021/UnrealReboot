// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/AI/Tasks/BTT_DefaultAttackCPP.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Actor.h"
#include "../../EnemyBase.h"

UBTT_DefaultAttackCPP::UBTT_DefaultAttackCPP()
{
   
}

EBTNodeResult::Type UBTT_DefaultAttackCPP::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    // AIController ��������
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to get AIController."));
        return EBTNodeResult::Failed;
    }

    // BlackboardComponent ��������
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComp)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to get BlackboardComponent."));
        return EBTNodeResult::Failed;
    }

    // ���� ��� ��������
    UObject* TargetObject = BlackboardComp->GetValueAsObject(AttackTargetKey.SelectedKeyName);
    if (!TargetObject)
    {
        UE_LOG(LogTemp, Warning, TEXT("No target found in Blackboard."));
        return EBTNodeResult::Failed;
    }

    // ��� ĳ���� (EnemyBase�� ĳ����)
    EnemyBase = Cast<AEnemyBase>(TargetObject);
    if (!EnemyBase)
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to cast target to AEnemyBase."));
        return EBTNodeResult::Failed;
    }

    // ���� ���� �̺�Ʈ ���ε� (���� ����)
    if (!EnemyBase->OnAttackEnd.IsAlreadyBound(this, &UBTT_DefaultAttackCPP::OnAttackFinished))
    {
        UE_LOG(LogTemp, Log, TEXT("Binding OnAttackEnd event."));
        EnemyBase->OnAttackEnd.AddDynamic(this, &UBTT_DefaultAttackCPP::OnAttackFinished);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("OnAttackEnd event is already bound."));
    }

    // OwnerComp ����
    CachedOwnerComp = &OwnerComp;

    // ���� ����
    AActor* TargetActor = Cast<AActor>(TargetObject);
    if (TargetActor)
    {
        UE_LOG(LogTemp, Log, TEXT("Executing Attack on target: %s"), *TargetActor->GetName());
        EnemyBase->Attack(TargetActor);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to cast target to AActor."));
        // �̺�Ʈ ���ε� ���� (���� ��)
        EnemyBase->OnAttackEnd.RemoveDynamic(this, &UBTT_DefaultAttackCPP::OnAttackFinished);
        return EBTNodeResult::Failed;
    }

    return EBTNodeResult::InProgress; // �۾��� �񵿱������� �����
}

void UBTT_DefaultAttackCPP::OnAttackFinished()
{
    if (CachedOwnerComp)
    {

        // �̺�Ʈ ���ε� ���� (�޸� ���� ����)
        if (EnemyBase)
        {
            EnemyBase->OnAttackEnd.RemoveDynamic(this, &UBTT_DefaultAttackCPP::OnAttackFinished);
        }

        FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
    }

}

