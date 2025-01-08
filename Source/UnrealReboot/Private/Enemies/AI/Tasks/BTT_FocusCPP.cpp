// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/AI/Tasks/BTT_FocusCPP.h"

UBTT_FocusCPP::UBTT_FocusCPP()
{
}

EBTNodeResult::Type UBTT_FocusCPP::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
   // AIController 가져오기
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        return EBTNodeResult::Failed;
    }

    // BlackboardComponent 가져오기
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComp)
    {
        return EBTNodeResult::Failed;
    }

    // FocusTargetKey의 값을 가져옴
    UObject* FocusTarget = BlackboardComp->GetValueAsObject(FocusTargetKey.SelectedKeyName);
    if (!FocusTarget)
    {
        return EBTNodeResult::Failed;
    }

    // FocusTarget이 Actor인지 확인
    if (AActor* FocusActor = Cast<AActor>(FocusTarget))
    {
        AIController->SetFocus(FocusActor);
    }
    else
    {
        // FocusTarget이 Vector로 설정된 경우
        FVector FocusLocation = BlackboardComp->GetValueAsVector(FocusTargetKey.SelectedKeyName);
        AIController->SetFocalPoint(FocusLocation);
    }

    return EBTNodeResult::Succeeded;
}
