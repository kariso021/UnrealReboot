// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/AI/Tasks/BTT_DefaultAttackCPP.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Actor.h"
#include "../../EnemyBase.h"

UBTT_DefaultAttackCPP::UBTT_DefaultAttackCPP()
{
   
}

EBTNodeResult::Type UBTT_DefaultAttackCPP::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

    // 공격 대상 가져오기
    UObject* TargetObject = BlackboardComp->GetValueAsObject(AttackTargetKey.SelectedKeyName);
    if (!TargetObject)
    {
        return EBTNodeResult::Failed;
    }

    // 대상 캐스팅 (EnemyBase로 캐스팅)
    EnemyBase = Cast<AEnemyBase>(TargetObject);
    if (!EnemyBase)
    {
        return EBTNodeResult::Failed;
    }

    AActor* TargetActor = Cast<AActor>(TargetObject);
    if (TargetActor)
    {
        // Attack 함수 호출
        EnemyBase->Attack(TargetActor);
    }

    // 공격 종료 이벤트 바인딩
    EnemyBase->OnAttackEnd.AddDynamic(this, &UBTT_DefaultAttackCPP::OnAttackFinished);

    // OwnerComp 저장
    CachedOwnerComp = &OwnerComp;

    return EBTNodeResult::InProgress; // 작업이 비동기적으로 진행됨
}

void UBTT_DefaultAttackCPP::OnAttackFinished()
{
    if (CachedOwnerComp)
    {
        FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);

        // 이벤트 바인딩 해제 (메모리 누수 방지)
        if (EnemyBase)
        {
            EnemyBase->OnAttackEnd.RemoveDynamic(this, &UBTT_DefaultAttackCPP::OnAttackFinished);
        }
    }

}

