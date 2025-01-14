// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/AI/Tasks/BTT_UnequipWeaponCPP.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Actor.h"
#include "../../EnemyBase.h"

EBTNodeResult::Type UBTT_UnequipWeaponCPP::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// OwnerComp ĳ��
	CachedOwnerComp = &OwnerComp;


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

	EnemyBase = Cast<AEnemyBase>(ControlledPawn);
	if (!EnemyBase)
	{
		return EBTNodeResult::Failed;
	}


	EnemyBase->UnequipWeapon();

	EnemyBase->OnWeaponUnEquipped.AddDynamic(this, &UBTT_UnequipWeaponCPP::OnFinishOnSheath);





	return EBTNodeResult::InProgress;
}

void UBTT_UnequipWeaponCPP::OnFinishOnSheath()
{
	if (CachedOwnerComp)
	{
		FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);

		// �̺�Ʈ ���ε� ���� (�޸� ���� ����)
		if (EnemyBase)
		{
			EnemyBase->OnAttackEnd.RemoveDynamic(this, &UBTT_UnequipWeaponCPP::OnFinishOnSheath);
		}
	}

}
