// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/AI/Decorators/BTD_HasPatrolRouteCPP.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "../../EnemyAIInterface.h"

bool UBTD_HasPatrolRouteCPP::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

    // Get the AI Controller
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        return false;
    }

    // Get the Controlled Pawn
    APawn* ControlledPawn = AIController->GetPawn();
    if (!ControlledPawn)
    {
        return false;
    }

    // Check if the Controlled Pawn implements the EnemyAIInterface
    if (ControlledPawn->GetClass()->ImplementsInterface(UEnemyAIInterface::StaticClass()))
    {
        // Call GetPatrolRoute() from the interface
        APatrolRoute* PatrolRoute = IEnemyAIInterface::Execute_GetPatrolRoute(ControlledPawn);

        // Check if the PatrolRoute is not null
        bool bHasPatrolRoute = (PatrolRoute != nullptr);
        return bHasPatrolRoute;
    }

    if (ControlledPawn->GetClass()->ImplementsInterface(UEnemyAIInterface::StaticClass()))
    {
        IEnemyAIInterface::Execute_EquipWeapon(ControlledPawn);

    }
    return false;



}
