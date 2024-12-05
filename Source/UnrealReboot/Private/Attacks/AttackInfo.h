#pragma once


#include "CoreMinimal.h"
#include "DamageSystem/DataOfDamage.h"
#include "Animation/AnimMontage.h"
#include "GameFramework/Actor.h"
#include "AttackInfo.generated.h"

USTRUCT(BlueprintType)
struct FAttackInfo
{
    GENERATED_USTRUCT_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Info")
    FDamageInfo DamageInfo;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Info")
    AActor* AttackTarget;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Info")
    UAnimMontage* Montage;
};