// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/AI/Tasks/BTT_MeleeAttackCPP.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Actor.h"
#include "../../EnemyAIInterface.h"
#include "../../Melee/EnemyMelee.h"
#include "../../DataOfEnemies.h"

UBTT_MeleeAttackCPP::UBTT_MeleeAttackCPP()
{
    TokenNeeded = 1;
}

EBTNodeResult::Type UBTT_MeleeAttackCPP::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    // AIController 가져오기
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        return EBTNodeResult::Failed;
    }

    // Controlled Pawn 가져오기
    APawn* ControlledPawn = AIController->GetPawn();
    if (!ControlledPawn)
    {
        return EBTNodeResult::Failed;
    }

    // EnemyMelee로 캐스팅
    EnemyMelee = Cast<AEnemyMelee>(ControlledPawn);
    if (!EnemyMelee)
    {
        return EBTNodeResult::Failed;
    }

    // IEnemyAIInterface를 구현했는지 확인
    if (!ControlledPawn->GetClass()->ImplementsInterface(UEnemyAIInterface::StaticClass()))
    {
        return EBTNodeResult::Failed;
    }

    // Blackboard 값 가져오기
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComp)
    {
        return EBTNodeResult::Failed;
    }

    UObject* TargetObject = BlackboardComp->GetValueAsObject(AttackTargetKey.SelectedKeyName);
    if (!TargetObject)
    {
        return EBTNodeResult::Failed;
    }

    AActor* AttackTarget = Cast<AActor>(TargetObject);
    if (!AttackTarget)
    {
        return EBTNodeResult::Failed;
    }

    // AttackRadiusKey에서 float 값 가져오기
    float AttackRadius = BlackboardComp->GetValueAsFloat(AttackRadiusKey.SelectedKeyName);
    if (AttackRadius <= 0.0f)
    {
        return EBTNodeResult::Failed; // 유효하지 않은 반경 값일 경우 실패 처리
    }

    // 공격 종료 이벤트 바인딩 (먼저 수행)
    if (!EnemyMelee->OnAttackEnd.IsAlreadyBound(this, &UBTT_MeleeAttackCPP::OnAttackFinished))
    {
        EnemyMelee->OnAttackEnd.AddDynamic(this, &UBTT_MeleeAttackCPP::OnAttackFinished);
    }

    // AttackStart 호출
    bool bAttackStarted = IEnemyAIInterface::Execute_AttackStart(ControlledPawn, AttackTarget, TokenNeeded);
    if (bAttackStarted)
    {
       /* AIController->ClearFocus(EAIFocusPriority::Gameplay);*/
       /* IEnemyAIInterface::Execute_SetMovementSpeed(ControlledPawn, EM_MovementSpeed::Sprinting);*/
    }
    else 
    {
        /*return EBTNodeResult::Failed;*/
    }

    // AttackStart가 정상 작동일 시
    IEnemyAIInterface::Execute_SetMovementSpeed(ControlledPawn, EM_MovementSpeed::Sprinting);
    AIController->ClearFocus(EAIFocusPriority::Gameplay);

    FVector TargetLocation = AttackTarget->GetActorLocation();
    EPathFollowingRequestResult::Type MoveResult = AIController->MoveToLocation(TargetLocation, AttackRadius, true);

    if (MoveResult == EPathFollowingRequestResult::RequestSuccessful || MoveResult == EPathFollowingRequestResult::AlreadyAtGoal)
    {
        AIController->SetFocus(AttackTarget);

        // 공격 실행
        if (EnemyMelee)
        {
            switch (AttackName)
            {
            case EM_Melee_Atttacks::Default:
                IEnemyAIInterface::Execute_Attack(ControlledPawn, AttackTarget);
                break;
            case EM_Melee_Atttacks::ShortRangeAttack:
                EnemyMelee->ShortRangeAttack(AttackTarget);
                break;
            case EM_Melee_Atttacks::LongRangeAttack:
                EnemyMelee->LongRangeAttack(AttackTarget);
                break;
            case EM_Melee_Atttacks::SpinningAttack:
                EnemyMelee->SpinningAttack(AttackTarget);
                break;
            case EM_Melee_Atttacks::GroundSmashAttack:
                EnemyMelee->GroundSmashAttack(AttackTarget);
                break;
            default:
                break;
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Controlled Pawn is not AEnemyMelee!"));
            return EBTNodeResult::Failed;
        }
    }
    else
    {
        // MoveToLocation 실패
        IEnemyAIInterface::Execute_AttackEnd(ControlledPawn, AttackTarget);

        // 이벤트 바인딩 해제 (실패 시)
        EnemyMelee->OnAttackEnd.RemoveDynamic(this, &UBTT_MeleeAttackCPP::OnAttackFinished);
        return EBTNodeResult::Failed;
    }

    // OwnerComp 캐시
    CachedOwnerComp = &OwnerComp;

    return EBTNodeResult::InProgress; // 비동기 작업 진행

}

void UBTT_MeleeAttackCPP::OnAttackFinished()
{
    if (CachedOwnerComp)
    {
        // 이벤트 바인딩 해제 (메모리 누수 방지)
        if (EnemyMelee)
        {
            EnemyMelee->OnAttackEnd.RemoveDynamic(this, &UBTT_MeleeAttackCPP::OnAttackFinished);
        }

        FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("CachedOwnerComp is null. Cannot finish task."));
    }

}
