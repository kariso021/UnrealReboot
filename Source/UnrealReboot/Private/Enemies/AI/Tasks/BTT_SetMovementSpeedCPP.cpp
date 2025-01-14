// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/AI/Tasks/BTT_SetMovementSpeedCPP.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Actor.h"
#include "../../EnemyAIInterface.h"

EBTNodeResult::Type UBTT_SetMovementSpeedCPP::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
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

	IEnemyAIInterface* AIInterface = Cast<IEnemyAIInterface>(ControlledPawn);
	if (!AIInterface)
	{
		return EBTNodeResult::Failed;
	}

	AIInterface->SetMovementSpeed(MovementSpeed);

	return EBTNodeResult::Succeeded;


}
