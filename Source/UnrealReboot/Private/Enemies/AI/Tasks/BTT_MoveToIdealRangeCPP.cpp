// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/AI/Tasks/BTT_MoveToIdealRangeCPP.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Actor.h"

EBTNodeResult::Type UBTT_MoveToIdealRangeCPP::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

    // Get Blackboard Values
    UObject* TargetObject = OwnerComp.GetBlackboardComponent()->GetValueAsObject(AttackTargetKey.SelectedKeyName);
    AActor* TargetActor = Cast<AActor>(TargetObject);

    float IdealRange = OwnerComp.GetBlackboardComponent()->GetValueAsFloat(IdealRangeKey.SelectedKeyName);

    if (!TargetActor)
    {
        return EBTNodeResult::Failed;
    }

    // 이동 작업 실행 (결과를 무시)
    if (TargetActor)
    {
        AIController->MoveToActor(TargetActor, IdealRange, true, true, false, nullptr, true);
    }

    // 무조건 성공 반환 버그생겨서 그냥 Succeeded 로 둠
    return EBTNodeResult::Succeeded;
}

