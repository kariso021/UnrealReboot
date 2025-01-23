// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_HealthBarCPP.generated.h"

/**
 * 
 */
UCLASS()
class UW_HealthBarCPP : public UUserWidget
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintCallable, Category = "Health")
	float GetPercent();

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health", meta = (AllowPrivateAccess = "true"))
	AActor* DamageableActor;

	
	
};
