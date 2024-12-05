// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemies/EnemyBase.h"
#include "GameFramework/Actor.h"
#include "EnemyRange.generated.h"

UCLASS()
class AEnemyRange : public AEnemyBase
{
	GENERATED_BODY()

public:
	// Constructor
	AEnemyRange();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Override any additional functionality or add new functions here
};
