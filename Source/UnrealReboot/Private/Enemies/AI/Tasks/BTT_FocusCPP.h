// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"                 
#include "BehaviorTree/BlackboardComponent.h" 
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_FocusCPP.generated.h"

/**
 * 
 */
UCLASS()
class UBTT_FocusCPP : public UBTTask_BlackboardBase
{
    GENERATED_BODY()

public:
    UBTT_FocusCPP();

    /** Main execution logic of the task */
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
    /** Blackboard Key Selector for the focus target (Actor) */
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector FocusTargetKey;



};

