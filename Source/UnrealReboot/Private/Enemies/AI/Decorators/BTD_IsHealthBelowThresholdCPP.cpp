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
        return false; // Pawn이 없는 경우 조건 실패
    }

    // Target이 IBPI_Damagable 인터페이스를 구현하는지 확인
    if (ControlledPawn->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()))
    {
        // MaxHealth 및 CurrentHealth 가져오기
        float MaxHealth = IDamageableInterface::Execute_GetMaxHealth(ControlledPawn);
        float CurrentHealth = IDamageableInterface::Execute_GetCurrentHealth(ControlledPawn);

        // Health Threshold와 비교
        if (CurrentHealth <= MaxHealth * (HealthThreshold / 100.0f))
        {
            return true; // 조건 충족
        }
    }

    return false; // 기본적으로 조건 실패
}
