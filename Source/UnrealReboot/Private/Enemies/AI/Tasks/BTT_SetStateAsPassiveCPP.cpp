// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/AI/Tasks/BTT_SetStateAsPassiveCPP.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Actor.h"
#include "../AIC_EnemyBaseCPP.h"

EBTNodeResult::Type UBTT_SetStateAsPassiveCPP::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	AAIC_EnemyBaseCPP* AIC_EnemyBase = Cast<AAIC_EnemyBaseCPP>(AIController);
	if (!AIC_EnemyBase)
	{
		return EBTNodeResult::Failed;
	}

	AIC_EnemyBase->SetStateAsPassive();

	return EBTNodeResult::Succeeded;


}
