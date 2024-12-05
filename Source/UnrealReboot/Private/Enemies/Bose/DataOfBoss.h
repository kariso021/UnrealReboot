// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EM_BossAttack : uint8
{
    Combo1 UMETA(DisplayName = "Combo1"),
    Combo2 UMETA(DisplayName = "Combo2"),
    ThrowMissile UMETA(DisplayName = "ThrowMissile"),
    JumpAttack UMETA(DisplayName = "JumpAttack"),
    GroundSmash UMETA(DisplayName = "GroundSmash"),
    AOETrail UMETA(DisplayName = "AOETrails"),
    DashGroundSmash UMETA(DisplayName = "DashGroundSmash"),
    BarrageSpell UMETA(DisplayName = "BarrageSpell"),
    QuickAttack UMETA(DisplayName = "QuickAttack")
};