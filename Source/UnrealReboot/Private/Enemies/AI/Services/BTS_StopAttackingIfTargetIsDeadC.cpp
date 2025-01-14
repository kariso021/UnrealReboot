// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/AI/Services/BTS_StopAttackingIfTargetIsDeadC.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Actor.h"
#include "../../../DamageSystem/DamageableInterface.h"
#include "../AIC_EnemyBaseCPP.h"

void UBTS_StopAttackingIfTargetIsDeadC::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    // ������ ������Ʈ ��������
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComp)
    {
        return;
    }

    // ��ǥ ��������
    AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(AttackTargetKey.SelectedKeyName));
    if (!TargetActor)
    {
        return;
    }

    // ��ǥ�� IDamageableInterface�� �����ϴ��� Ȯ��
    if (TargetActor->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()))
    {
        bool bIsDead = IDamageableInterface::Execute_IsDead(TargetActor);

        // ��ǥ�� �׾��ٸ� AI ���� ����
        if (bIsDead)
        {
            AAIC_EnemyBaseCPP* AIController = Cast<AAIC_EnemyBaseCPP>(OwnerComp.GetAIOwner());
            if (AIController)
            {
                AIController->SetStateAsPassive();
            }
        }
    }

}
