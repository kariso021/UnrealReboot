// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/AI/Tasks/BTT_SetIsInterruptibleCPP.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Actor.h"
#include "../../../DamageSystem/DamageableInterface.h"

EBTNodeResult::Type UBTT_SetIsInterruptibleCPP::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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
    if (ControlledPawn->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()))
    {
        // SetIsInterruptible ȣ��
        IDamageableInterface::Execute_SetIsInterruptible(ControlledPawn, Value);

        // �۾� ���� ��ȯ
        return EBTNodeResult::Succeeded;
    }

    // �������̽��� �������� ���� ��� ���� ��ȯ
    return EBTNodeResult::Failed;
}
