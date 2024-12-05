// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AOEBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAOEOverlapActor,AActor*, DamageCauser);


UCLASS()
class AAOEBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAOEBase();

	UPROPERTY(BlueprintAssignable, Category = "OnAOEOver")
	FOnAOEOverlapActor OnAOEOverlapActor;

	UFUNCTION()
	void SpawnAOESphere();

	UFUNCTION()
	void Trigger();

	UFUNCTION()
	void SetRadius(float Rad);



private:
	float radius;
	bool bDrawDebugSphere;
	bool ignoreInstigator;
	bool triggerOnBeginPlay;






protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	



};
