// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Perception/AISense.h"
#include "Perception/AIPerceptionTypes.h"
#include "Engine/UserDefinedEnum.h"
#include "DataOfEnemies.generated.h"

UENUM(BlueprintType)
enum class EM_AISense : uint8
{
    None UMETA(DisplayName = "None"),
    Sight UMETA(DisplayName = "Sight"),
    Hearing UMETA(DisplayName = "Hearing"),
    Damage UMETA(DisplayName = "Damage")
};

UENUM(BlueprintType)
enum class EM_AIState : uint8
{
    Passive UMETA(DisplayName = "Passive"),
    Attacking UMETA(DisplayName = " Attacking"),
    Frozen UMETA(DisplayName = "Frozen"),
    Investigating UMETA(DisplayName = "Investigating"),
    Dead UMETA(DisplayName = "Dead"),
    Seeking UMETA(DisplayName = "Seeking")
};

UENUM(BlueprintType)
enum class EM_MovementSpeed : uint8
{
    Idle UMETA(DisplayName = "Idle"),
    Walk UMETA(DisplayName = "Walk"),
    Jogging UMETA(DisplayName = "Jogging"),
    Sprinting UMETA(DisplayName = "Sprinting")
};


USTRUCT(Atomic, BlueprintType)
struct FRangeofState
{
    GENERATED_USTRUCT_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IdealRange")
    float AttackRadius;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IdealRange")
    float DefendRadius;

};

USTRUCT(Atomic, BlueprintType)
struct FCheckSensedStimulus
{
    GENERATED_USTRUCT_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stimulus")
    FAIStimulus Stimulus;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sensed")
    bool Sensed;

};