// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile/ProjectileBase.h"
#include "DataOfDamage.h"
//ProjectileDamageInfoMaaping
#include "DamageInfoMapping.generated.h"



USTRUCT(BlueprintType)
struct FDamageInfoMapping
{
    GENERATED_BODY()

public:
    UPROPERTY()
    AProjectileBase* Projectile;

    UPROPERTY()
    FDamageInfo DamageInfo;

    FDamageInfoMapping() : Projectile(nullptr) {}

    FDamageInfoMapping(AProjectileBase* InProjectile, FDamageInfo InDamageInfo)
        : Projectile(InProjectile), DamageInfo(InDamageInfo) {}
};