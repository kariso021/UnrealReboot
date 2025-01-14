// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/AI/Decorators/BTD_IsWieldingSwordCPP.h"
#include "AIController.h"
#include"../../EnemyBase.h"

bool UBTD_IsWieldingSwordCPP::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    // AIController �� Controlled Pawn ��������
    AAIController* AIController = OwnerComp.GetAIOwner();
    APawn* ControlledPawn = AIController ? AIController->GetPawn() : nullptr;

    if (!ControlledPawn)
    {
        return false; // Pawn�� ������ ���� ����
    }

    // ControlledPawn�� BP_Enemy_Base�� ĳ����
    AEnemyBase* EnemyBase = Cast<AEnemyBase>(ControlledPawn);
    if (!EnemyBase)
    {
        return false; // ĳ���� ���� �� ���� ����
    }

    // Is Wielding Weapon Ȯ��
    return EnemyBase->GetisWieldingWeapon();
}
