// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/AI/Tasks/BTT_MeleeBlockCPP.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Actor.h"
#include "../../EnemyAIInterface.h"
#include "../../Melee/EnemyMelee.h"


EBTNodeResult::Type UBTT_MeleeBlockCPP::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

    CachedOwnerComp = &OwnerComp;
    APawn* ControlledPawn = CachedAIController->GetPawn();
    if (!ControlledPawn)
    {
        return EBTNodeResult::Failed;
    }

    EnemyBase = Cast<AEnemyMelee>(ControlledPawn);
    if (!EnemyBase)
    {
        return EBTNodeResult::Failed;
    }

    EnemyBase->StartBlock();

  

    EnemyBase->OnBlockEnded.AddDynamic(this, &UBTT_MeleeBlockCPP::OnBlockFinished);



    return EBTNodeResult::InProgress; // �񵿱� �۾� ����

}

void UBTT_MeleeBlockCPP::OnBlockFinished()
{
    if (CachedOwnerComp)
    {
        FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);

        // �̺�Ʈ ���ε� ���� (�޸� ���� ����)
        if (EnemyBase)
        {
            EnemyBase->OnAttackEnd.RemoveDynamic(this, &UBTT_MeleeBlockCPP::OnBlockFinished);
        }
    }
}
