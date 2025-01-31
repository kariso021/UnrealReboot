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
    // AIController ��������
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        return EBTNodeResult::Failed;
    }

    // Controlled Pawn ��������
    APawn* ControlledPawn = AIController->GetPawn();
    if (!ControlledPawn)
    {
        return EBTNodeResult::Failed;
    }

    // EnemyMelee�� ĳ����
    EnemyMelee = Cast<AEnemyMelee>(ControlledPawn);
    if (!EnemyMelee)
    {
        return EBTNodeResult::Failed;
    }

    // IEnemyAIInterface�� �����ߴ��� Ȯ��
    if (!ControlledPawn->GetClass()->ImplementsInterface(UEnemyAIInterface::StaticClass()))
    {
        return EBTNodeResult::Failed;
    }

    // Blackboard �� ��������
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

    // AttackRadiusKey���� float �� ��������
    float AttackRadius = BlackboardComp->GetValueAsFloat(AttackRadiusKey.SelectedKeyName);
    if (AttackRadius <= 0.0f)
    {
        return EBTNodeResult::Failed; // ��ȿ���� ���� �ݰ� ���� ��� ���� ó��
    }

    // ���� ���� �̺�Ʈ ���ε� (���� ����)
    if (!EnemyMelee->OnAttackEnd.IsAlreadyBound(this, &UBTT_MeleeAttackCPP::OnAttackFinished))
    {
        EnemyMelee->OnAttackEnd.AddDynamic(this, &UBTT_MeleeAttackCPP::OnAttackFinished);
    }

    // AttackStart ȣ��
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

    // AttackStart�� ���� �۵��� ��
    IEnemyAIInterface::Execute_SetMovementSpeed(ControlledPawn, EM_MovementSpeed::Sprinting);
    AIController->ClearFocus(EAIFocusPriority::Gameplay);

    FVector TargetLocation = AttackTarget->GetActorLocation();
    EPathFollowingRequestResult::Type MoveResult = AIController->MoveToLocation(TargetLocation, AttackRadius, true);

    if (MoveResult == EPathFollowingRequestResult::RequestSuccessful || MoveResult == EPathFollowingRequestResult::AlreadyAtGoal)
    {
        AIController->SetFocus(AttackTarget);

        // ���� ����
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
        // MoveToLocation ����
        IEnemyAIInterface::Execute_AttackEnd(ControlledPawn, AttackTarget);

        // �̺�Ʈ ���ε� ���� (���� ��)
        EnemyMelee->OnAttackEnd.RemoveDynamic(this, &UBTT_MeleeAttackCPP::OnAttackFinished);
        return EBTNodeResult::Failed;
    }

    // OwnerComp ĳ��
    CachedOwnerComp = &OwnerComp;

    return EBTNodeResult::InProgress; // �񵿱� �۾� ����

}

void UBTT_MeleeAttackCPP::OnAttackFinished()
{
    if (CachedOwnerComp)
    {
        // �̺�Ʈ ���ε� ���� (�޸� ���� ����)
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
