// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/AI/Tasks/BTT_SetIsInvincibleCPP.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Actor.h"
#include "../../../DamageSystem/DamageableInterface.h"

EBTNodeResult::Type UBTT_SetIsInvincibleCPP::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    // AI Controller 가져오기
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) // Null 체크
    {
        return EBTNodeResult::Failed;
    }

    // Controlled Pawn 가져오기
    APawn* ControlledPawn = AIController->GetPawn();
    if (!ControlledPawn) // Null 체크
    {
        return EBTNodeResult::Failed;
    }

    // ControlledPawn이 IDamageableInterface를 구현했는지 확인
    if (!ControlledPawn->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()))
    {
        return EBTNodeResult::Failed;
    }

    // SetIsInvincible 호출
    IDamageableInterface::Execute_SetIsInvincible(ControlledPawn, Value);

    // 작업 성공 반환
    return EBTNodeResult::Succeeded;
}