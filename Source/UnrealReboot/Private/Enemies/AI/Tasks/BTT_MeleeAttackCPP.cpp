// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/AI/Tasks/BTT_MeleeAttackCPP.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Actor.h"
#include "../../EnemyBase.h"

EBTNodeResult::Type UBTT_MeleeAttackCPP::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    // AIController ��������
    CachedAIController = OwnerComp.GetAIOwner();
    if (!CachedAIController)
    {
        return EBTNodeResult::Failed;
    }

    // Controlled Pawn ��������
    APawn* ControlledPawn = CachedAIController->GetPawn();
    if (!ControlledPawn)
    {
        return EBTNodeResult::Failed;
    }

    // EnemyBase�� ĳ����
    EnemyBase = Cast<AEnemyBase>(ControlledPawn);
    if (!EnemyBase)
    {
        return EBTNodeResult::Failed;
    }

    // Blackboard �� ��������
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComp)
    {
        return EBTNodeResult::Failed;
    }

    UObject* TargetObject = BlackboardComp->GetValueAsObject(AttackTargetKey.SelectedKeyName);
    if (!TargetObject)
    {
        return EBTNodeResult::Failed;
    }

    AActor* AttackTarget = Cast<AActor>(TargetObject);
    if (!AttackTarget)
    {
        return EBTNodeResult::Failed;
    }





    EnemyBase->OnAttackEnd.AddDynamic(this, &UBTT_MeleeAttackCPP::OnAttackFinished);

    // OwnerComp ĳ��
    CachedOwnerComp = &OwnerComp;

    return EBTNodeResult::InProgress; // �񵿱� �۾� ����

}

void UBTT_MeleeAttackCPP::OnAttackFinished()
{


}
