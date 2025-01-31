// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/W_HealthBarCPP.h"
#include "DamageSystem/DamageableInterface.h"

float UW_HealthBarCPP::GetPercent()
{
    if (!DamageableActor) // DamageableActor가 null인지 확인
    {
        return 0.0f;
    }

    // DamageableActor가 IDamageableInterface를 구현했는지 확인
    if (DamageableActor->Implements<UDamageableInterface>())
    {
        const float CurrentHealth = IDamageableInterface::Execute_GetCurrentHealth(DamageableActor);
        const float MaxHealth = IDamageableInterface::Execute_GetMaxHealth(DamageableActor);

        if (MaxHealth > 0.0f) // 최대 체력이 0보다 클 경우에만 계산
        {
            return CurrentHealth / MaxHealth;
        }
    }

    return 0.0f; // 인터페이스를 구현하지 않은 경우
}

void UW_HealthBarCPP::SetDamageableActor(AActor* actor)
{
    DamageableActor = actor;
}
