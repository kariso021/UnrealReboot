// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/AI/Tasks/BTT_SetIsInvincibleCPP.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Actor.h"
#include "../../../DamageSystem/DamageableInterface.h"

EBTNodeResult::Type UBTT_SetIsInvincibleCPP::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    // AI Controller ��������
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) // Null üũ
    {
        return EBTNodeResult::Failed;
    }

    // Controlled Pawn ��������
    APawn* ControlledPawn = AIController->GetPawn();
    if (!ControlledPawn) // Null üũ
    {
        return EBTNodeResult::Failed;
    }

    // ControlledPawn�� IDamageableInterface�� �����ߴ��� Ȯ��
    if (!ControlledPawn->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()))
    {
        return EBTNodeResult::Failed;
    }

    // SetIsInvincible ȣ��
    IDamageableInterface::Execute_SetIsInvincible(ControlledPawn, Value);

    // �۾� ���� ��ȯ
    return EBTNodeResult::Succeeded;
}