// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/AI/Tasks/BTT_UnequipWeaponCPP.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Actor.h"
#include "../../EnemyBase.h"

EBTNodeResult::Type UBTT_UnequipWeaponCPP::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// OwnerComp 캐시
	CachedOwnerComp = &OwnerComp;


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

	EnemyBase = Cast<AEnemyBase>(ControlledPawn);
	if (!EnemyBase)
	{
		return EBTNodeResult::Failed;
	}

	// EquipWeapon 함수 호출 (인터페이스 사용)
	if (EnemyBase->GetClass()->ImplementsInterface(UEnemyAIInterface::StaticClass()))
	{
		IEnemyAIInterface::Execute_UnequipWeapon(EnemyBase);
	}

	// Weapon Equipped 이벤트 바인딩
	EnemyBase->OnWeaponUnEquipped.AddDynamic(this, &UBTT_UnequipWeaponCPP::OnFinishOnSheath);





	return EBTNodeResult::InProgress;
}

void UBTT_UnequipWeaponCPP::OnFinishOnSheath()
{
	if (CachedOwnerComp)
	{
		FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);

		// 이벤트 바인딩 해제 (메모리 누수 방지)
		if (EnemyBase)
		{
			EnemyBase->OnAttackEnd.RemoveDynamic(this, &UBTT_UnequipWeaponCPP::OnFinishOnSheath);
		}
	}

}
