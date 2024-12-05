// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include"DataOfDamage.generated.h"

UENUM(BlueprintType)
enum class EM_DamageType : uint8
{
    None UMETA(DisplayName = "None"),
    Melee UMETA(DisplayName = "Melee"),
    Explosion UMETA(DisplayName = "Explosion"),
    Projectile UMETA(DisplayName = "Projectile"),
    Environment UMETA(DisplayName = "Environment")
};

UENUM(BlueprintType)
enum class EM_DamageResponse : uint8
{
    None UMETA(DisplayName = "None"),
    HitReaction UMETA(DisplayName = "HitReaction"),
    Stagger UMETA(DisplayName = "Stagger"),
    Stun UMETA(DisplayName = "Stun"),
    KnockBack UMETA(DisplayName = "KnockBack")
};

USTRUCT(Atomic, BlueprintType)
struct FDamageInfo
{
    GENERATED_USTRUCT_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Info")
    float Amount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Info")
    EM_DamageType DamageType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Info")
    EM_DamageResponse DamageResponse;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Info")
    bool ShouldDamageInvincible;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Info")
    bool SCanBeBlocked;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Info")
    bool CanBeParried;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Info")
    bool ShouldForceInterrupt;



};