// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PatrolRoute.generated.h"

UCLASS()
class APatrolRoute : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APatrolRoute();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Patrol")
	FVector GetSplinePointAsWorldPosition() const;

	UFUNCTION(BlueprintCallable, Category = "Patrol")
	void IncrementPatrolRoute();


	// Spline component for the patrol route
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Patrol")
	class USplineComponent* PatrolRoute;

	// Patrol index
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Patrol")
	int32 PatrolIndex;

	// Patrol direction (1 for forward, -1 for backward)
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Patrol")
	int32 Direction;

};
