// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/AI/Decorators/BTD_IsHealthBelowThresholdCPP.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "../../../DamageSystem/DamageableInterface.h"

bool UBTD_IsHealthBelowThresholdCPP::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    APawn* ControlledPawn = AIController ? AIController->GetPawn() : nullptr;

    if (!ControlledPawn)
    {
        return false; // Pawn�� ���� ��� ���� ����
    }

    // Target�� IBPI_Damagable �������̽��� �����ϴ��� Ȯ��
    if (ControlledPawn->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()))
    {
        // MaxHealth �� CurrentHealth ��������
        float MaxHealth = IDamageableInterface::Execute_GetMaxHealth(ControlledPawn);
        float CurrentHealth = IDamageableInterface::Execute_GetCurrentHealth(ControlledPawn);

        // Health Threshold�� ��
        if (CurrentHealth <= MaxHealth * (HealthThreshold / 100.0f))
        {
            return true; // ���� ����
        }
    }

    return false; // �⺻������ ���� ����
}
