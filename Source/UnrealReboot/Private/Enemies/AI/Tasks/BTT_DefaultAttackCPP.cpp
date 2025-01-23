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
        UE_LOG(LogTemp, Error, TEXT("Failed to get AIController."));
        return EBTNodeResult::Failed;
    }

    // BlackboardComponent 가져오기
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComp)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to get BlackboardComponent."));
        return EBTNodeResult::Failed;
    }

    // 공격 대상 가져오기
    UObject* TargetObject = BlackboardComp->GetValueAsObject(AttackTargetKey.SelectedKeyName);
    if (!TargetObject)
    {
        UE_LOG(LogTemp, Warning, TEXT("No target found in Blackboard."));
        return EBTNodeResult::Failed;
    }

    // 대상 캐스팅 (EnemyBase로 캐스팅)
    EnemyBase = Cast<AEnemyBase>(TargetObject);
    if (!EnemyBase)
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to cast target to AEnemyBase."));
        return EBTNodeResult::Failed;
    }

    // 공격 종료 이벤트 바인딩 (먼저 수행)
    if (!EnemyBase->OnAttackEnd.IsAlreadyBound(this, &UBTT_DefaultAttackCPP::OnAttackFinished))
    {
        UE_LOG(LogTemp, Log, TEXT("Binding OnAttackEnd event."));
        EnemyBase->OnAttackEnd.AddDynamic(this, &UBTT_DefaultAttackCPP::OnAttackFinished);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("OnAttackEnd event is already bound."));
    }

    // OwnerComp 저장
    CachedOwnerComp = &OwnerComp;

    // 공격 수행
    AActor* TargetActor = Cast<AActor>(TargetObject);
    if (TargetActor)
    {
        UE_LOG(LogTemp, Log, TEXT("Executing Attack on target: %s"), *TargetActor->GetName());
        EnemyBase->Attack(TargetActor);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to cast target to AActor."));
        // 이벤트 바인딩 해제 (실패 시)
        EnemyBase->OnAttackEnd.RemoveDynamic(this, &UBTT_DefaultAttackCPP::OnAttackFinished);
        return EBTNodeResult::Failed;
    }

    return EBTNodeResult::InProgress; // 작업이 비동기적으로 진행됨
}

void UBTT_DefaultAttackCPP::OnAttackFinished()
{
    if (CachedOwnerComp)
    {

        // 이벤트 바인딩 해제 (메모리 누수 방지)
        if (EnemyBase)
        {
            EnemyBase->OnAttackEnd.RemoveDynamic(this, &UBTT_DefaultAttackCPP::OnAttackFinished);
        }

        FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
    }

}

