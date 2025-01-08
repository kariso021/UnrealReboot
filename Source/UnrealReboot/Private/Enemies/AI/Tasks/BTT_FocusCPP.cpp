// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/AI/Tasks/BTT_FocusCPP.h"

UBTT_FocusCPP::UBTT_FocusCPP()
{
}

EBTNodeResult::Type UBTT_FocusCPP::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
   // AIController ��������
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        return EBTNodeResult::Failed;
    }

    // BlackboardComponent ��������
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComp)
    {
        return EBTNodeResult::Failed;
    }

    // FocusTargetKey�� ���� ������
    UObject* FocusTarget = BlackboardComp->GetValueAsObject(FocusTargetKey.SelectedKeyName);
    if (!FocusTarget)
    {
        return EBTNodeResult::Failed;
    }

    // FocusTarget�� Actor���� Ȯ��
    if (AActor* FocusActor = Cast<AActor>(FocusTarget))
    {
        AIController->SetFocus(FocusActor);
    }
    else
    {
        // FocusTarget�� Vector�� ������ ���
        FVector FocusLocation = BlackboardComp->GetValueAsVector(FocusTargetKey.SelectedKeyName);
        AIController->SetFocalPoint(FocusLocation);
    }

    return EBTNodeResult::Succeeded;
}
