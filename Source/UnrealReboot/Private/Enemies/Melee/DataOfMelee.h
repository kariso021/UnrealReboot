// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataOfMelee.generated.h"
/**
 * 
 */

//Default ,ShortRangeAttack, LongRangeAttack , SpinningAttack, GroundSmashAttack


//and also have BlockingState


UENUM(BlueprintType)
enum class EM_Melee_Atttacks : uint8
{
    Default UMETA(DisplayName = "Default"),
    ShortRangeAttack UMETA(DisplayName = "ShortRangeAttack"),
    LongRangeAttack UMETA(DisplayName = "LongRangeAttack"),
    SpinningAttack UMETA(DisplayName = "SpinningAttack"),
    GroundSmashAttack UMETA(DisplayName = "GroundSmashAttack")
};

UENUM(BlueprintType)
enum class EM_BlockingState : uint8
{
    None UMETA(DisplayName = "None"),
    Blocking UMETA(DisplayName = "Blocking"),
    BlockedSucessfully UMETA(DisplayName = "BlockedSucessfully")
};