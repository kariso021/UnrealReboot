// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Player_HUD_CPP.generated.h"



// ���� ���� (AMainPlayer Ŭ������ �̸� �˷���)
class AMainPlayer;
/**
 * 
 */
UCLASS()
class UPlayer_HUD_CPP : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetPlayerReference(AMainPlayer* mainplayer);



private:
	UPROPERTY()
	AMainPlayer* OwnerPlayer;

	UFUNCTION(BlueprintCallable)
	float GetHealthPercent();

	
	
};
