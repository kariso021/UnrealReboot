// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DamageSystem/DamageableInterface.h"
#include "Blueprint/UserWidget.h"
#include "WD_HealthBar.generated.h"

/**
 * 
 */
UCLASS()
class UWD_HealthBar : public UUserWidget
{
	GENERATED_BODY()


public:
	UFUNCTION()
	float GetPercent();

	UFUNCTION()
	void SetDamageableActor(const TScriptInterface<IDamageableInterface>& InDamageableActor);



private:

	TScriptInterface<IDamageableInterface> DamageableActor;
};
