// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/AI/Tasks/BTT_MoveAlongPatrolRouteCPP.h"
#include "../../PatrolRoute.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Actor.h"
#include "../../EnemyAIInterface.h"

EBTNodeResult::Type UBTT_MoveAlongPatrolRouteCPP::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    // Get AI Controller and Controlled Pawn
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        return EBTNodeResult::Failed;
    }

    APawn* ControlledPawn = AIController->GetPawn();
    if (!ControlledPawn)
    {
        return EBTNodeResult::Failed;
    }

    // Check if ControlledPawn implements the interface
    if (ControlledPawn->GetClass()->ImplementsInterface(UEnemyAIInterface::StaticClass()))
    {
        // Get Patrol Route from the interface
        APatrolRoute* PatrolRoute = IEnemyAIInterface::Execute_GetPatrolRoute(ControlledPawn);
        if (!PatrolRoute)
        {
            UE_LOG(LogTemp, Warning, TEXT("PatrolRoute is null!"));
            return EBTNodeResult::Failed;
        }

        // Get the next patrol point as a world position
        FVector Destination = PatrolRoute->GetSplinePointAsWorldPosition();

        // Move to the location
        FAIRequestID RequestID = AIController->MoveToLocation(Destination, 10.0f);
        if (RequestID.IsValid())
        {
            // Increment the patrol route after successfully initiating the move
            PatrolRoute->IncrementPatrolRoute();

            // Successfully initiated move
            return EBTNodeResult::Succeeded;
        }
        else
        {
            // Failed to initiate move
            UE_LOG(LogTemp, Warning, TEXT("MoveToLocation failed!"));
            return EBTNodeResult::Failed;
        }
    }

    // Controlled pawn does not implement the interface
    return EBTNodeResult::Failed;
}



EBTNodeResult::Type UBTT_MoveAlongPatrolRouteCPP::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    // Get AI Controller
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (AIController)
    {
        // Stop AI movement
        AIController->StopMovement();
    }

    // Finish the abort process
    return EBTNodeResult::Aborted; // Return the abort result
}

