// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/AI/Tasks/BTT_EquipWeaponCPP.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../../EnemyBase.h"

EBTNodeResult::Type UBTT_EquipWeaponCPP::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    // AIController ��������
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        return EBTNodeResult::Failed;
    }

    // Controlled Pawn ��������
    APawn* ControlledPawn = AIController->GetPawn();
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

    // EquipWeapon �Լ� ȣ��
    EnemyBase->EquipWeapon();

    // Weapon Equipped �̺�Ʈ ���ε�
    EnemyBase->OnWeaponEquipped.AddDynamic(this, &UBTT_EquipWeaponCPP::OnWeaponEquipped);

    // OwnerComp ����
    CachedOwnerComp = &OwnerComp;

    return EBTNodeResult::InProgress; // �񵿱� �۾��� ���� ����
}

void UBTT_EquipWeaponCPP::OnWeaponEquipped()
{
    if (CachedOwnerComp)
    {
        FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);

        // �̺�Ʈ ���ε� ���� (�޸� ���� ����)
        if (EnemyBase)
        {
            EnemyBase->OnWeaponEquipped.RemoveDynamic(this, &UBTT_EquipWeaponCPP::OnWeaponEquipped);
        }
    }

}
