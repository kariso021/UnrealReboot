// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/AI/Decorators/BTD_IisTargetAttackingCPP.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../../../DamageSystem/DamageableInterface.h"

bool UBTD_IisTargetAttackingCPP::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    // Get Blackboard and Target Actor
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComp)
    {
        return false; // ���� ����
    }

    AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(AttackTargetKey.SelectedKeyName));
    if (!TargetActor)
    {
        return false; // ��� Actor�� ������ ���� ����
    }

    // Check if the target implements the DamageableInterface
    if (TargetActor->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()))
    {
        // Call IsAttacking via the interface
        bool bIsAttacking = IDamageableInterface::Execute_IsAttacking(TargetActor);
        return bIsAttacking; // ���� ������ ��ȯ
    }

    return false; // �������̽��� �������� ������ ���� ����
}
