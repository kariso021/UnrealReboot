// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/PatrolRoute.h"
#include "Components/SplineComponent.h"

// Sets default values
APatrolRoute::APatrolRoute()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PatrolRoute = CreateDefaultSubobject<USplineComponent>(TEXT("PatrolRoute"));
	PatrolRoute->SetupAttachment(RootComponent);

	// Initialize patrol index and direction
	PatrolIndex = 0;
	Direction = 1; // Start by moving forward

}

// Called when the game starts or when spawned
void APatrolRoute::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APatrolRoute::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FVector APatrolRoute::GetSplinePointAsWorldPosition() const
{
	return PatrolRoute->GetLocationAtSplinePoint(PatrolIndex, ESplineCoordinateSpace::World);
}

void APatrolRoute::IncrementPatrolRoute()
{
    if (!PatrolRoute)
    {
        UE_LOG(LogTemp, Warning, TEXT("PatrolRoute is null!"));
        return;
    }

    int32 NumPoints = PatrolRoute->GetNumberOfSplinePoints();

    // Update the patrol index
    PatrolIndex += Direction;

    // Check if we need to reverse direction
    if (PatrolIndex >= NumPoints)
    {
        PatrolIndex = NumPoints - 1; // Clamp to last point
        Direction = -1; // Reverse direction
    }
    else if (PatrolIndex < 0)
    {
        PatrolIndex = 0; // Clamp to first point
        Direction = 1; // Reverse direction
    }
}

