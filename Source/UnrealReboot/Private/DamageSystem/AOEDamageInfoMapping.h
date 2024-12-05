// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataOfDamage.h"
#include "AOE/AOEBase.h"
#include "AOEDamageInfoMapping.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FAOEDamageInfoMapping
{
    GENERATED_BODY()

public:
    UPROPERTY()
    AActor* AOEDamageActor;

    UPROPERTY()
    FDamageInfo DamageInfo;

    FAOEDamageInfoMapping() : AOEDamageActor(nullptr) {}

    FAOEDamageInfoMapping(AActor* InAOEDamageActor, FDamageInfo InDamageInfo)
        : AOEDamageActor(InAOEDamageActor), DamageInfo(InDamageInfo) {}
};
