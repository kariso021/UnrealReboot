// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "../../DataOfEnemies.h"
#include "BTT_SetMovementSpeedCPP.generated.h"

/**
 * 
 */
UCLASS()
class UBTT_SetMovementSpeedCPP : public UBTTaskNode
{
	GENERATED_BODY()


public:


protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard", meta = (AllowPrivateAccess = "true", DisplayName = "MovementSpeed"))
	EM_MovementSpeed MovementSpeed;
	
};
