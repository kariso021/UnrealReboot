// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/W_HealthBarCPP.h"
#include "DamageSystem/DamageableInterface.h"

float UW_HealthBarCPP::GetPercent()
{
    if (!DamageableActor) // DamageableActor�� null���� Ȯ��
    {
        return 0.0f;
    }

    // DamageableActor�� IDamageableInterface�� �����ߴ��� Ȯ��
    if (DamageableActor->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()))
    {
        const float CurrentHealth = IDamageableInterface::Execute_GetCurrentHealth(DamageableActor);
        const float MaxHealth = IDamageableInterface::Execute_GetMaxHealth(DamageableActor);

        if (MaxHealth > 0.0f) // �ִ� ü���� 0���� Ŭ ��쿡�� ���
        {
            return CurrentHealth / MaxHealth;
        }
    }

    return 0.0f; // �������̽��� �������� ���� ���
}
