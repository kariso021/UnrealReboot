// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/AI/Tasks/BTT_ClearFocusCPP.h"
#include "AIController.h"

UBTT_ClearFocusCPP::UBTT_ClearFocusCPP()
{
    NodeName = TEXT("Clear Focus");
}

EBTNodeResult::Type UBTT_ClearFocusCPP::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    // AIController 가져오기
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        return EBTNodeResult::Failed;
    }

    // Clear Focus 호출
    AIController->ClearFocus(EAIFocusPriority::Gameplay);

    // 작업 성공 처리
    return EBTNodeResult::Succeeded;
}
