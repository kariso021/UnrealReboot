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
        return EBTNodeResult::Failed;
    }

    // BlackboardComponent ��������
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComp)
    {
        return EBTNodeResult::Failed;
    }

    // ���� ��� ��������
    UObject* TargetObject = BlackboardComp->GetValueAsObject(AttackTargetKey.SelectedKeyName);
    if (!TargetObject)
    {
        return EBTNodeResult::Failed;
    }

    // ��� ĳ���� (EnemyBase�� ĳ����)
    EnemyBase = Cast<AEnemyBase>(TargetObject);
    if (!EnemyBase)
    {
        return EBTNodeResult::Failed;
    }

    AActor* TargetActor = Cast<AActor>(TargetObject);
    if (TargetActor)
    {
        // Attack �Լ� ȣ��
        EnemyBase->Attack(TargetActor);
    }

    // ���� ���� �̺�Ʈ ���ε�
    EnemyBase->OnAttackEnd.AddDynamic(this, &UBTT_DefaultAttackCPP::OnAttackFinished);

    // OwnerComp ����
    CachedOwnerComp = &OwnerComp;

    return EBTNodeResult::InProgress; // �۾��� �񵿱������� �����
}

void UBTT_DefaultAttackCPP::OnAttackFinished()
{
    if (CachedOwnerComp)
    {
        FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);

        // �̺�Ʈ ���ε� ���� (�޸� ���� ����)
        if (EnemyBase)
        {
            EnemyBase->OnAttackEnd.RemoveDynamic(this, &UBTT_DefaultAttackCPP::OnAttackFinished);
        }
    }

}

