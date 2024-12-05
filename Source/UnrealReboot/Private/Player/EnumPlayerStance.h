// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnumPlayerStance.generated.h"

UENUM(BlueprintType)
enum class EPlayerStance : uint8
{
    Unarmed UMETA(DisplayName = "Unarmed"),
    Ranged UMETA(DisplayName = "Ranged"),
    Melee UMETA(DisplayName = "Melee")
};