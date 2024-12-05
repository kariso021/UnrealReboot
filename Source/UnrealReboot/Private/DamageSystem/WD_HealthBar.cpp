// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageSystem/WD_HealthBar.h"

float UWD_HealthBar::GetPercent()
{
    if (DamageableActor)
    {
        float MaxHealth = DamageableActor->GetMaxHealth();
        float CurrentHealth = DamageableActor->GetCurrentHealth();
        if (MaxHealth > 0)
        {
            return CurrentHealth / MaxHealth;
        }
    }
    return 0.0f;
}

void UWD_HealthBar::SetDamageableActor(const TScriptInterface<IDamageableInterface>& InDamageableActor)
{
    DamageableActor = InDamageableActor;
}