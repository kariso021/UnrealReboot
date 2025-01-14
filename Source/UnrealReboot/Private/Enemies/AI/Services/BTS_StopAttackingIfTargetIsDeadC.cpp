// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/AI/Services/BTS_StopAttackingIfTargetIsDeadC.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Actor.h"
#include "../../../DamageSystem/DamageableInterface.h"
#include "../AIC_EnemyBaseCPP.h"

void UBTS_StopAttackingIfTargetIsDeadC::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    // 블랙보드 컴포넌트 가져오기
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComp)
    {
        return;
    }

    // 목표 가져오기
    AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(AttackTargetKey.SelectedKeyName));
    if (!TargetActor)
    {
        return;
    }

    // 목표가 IDamageableInterface를 구현하는지 확인
    if (TargetActor->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()))
    {
        bool bIsDead = IDamageableInterface::Execute_IsDead(TargetActor);

        // 목표가 죽었다면 AI 상태 변경
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
