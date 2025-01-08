// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/AI/Tasks/BTT_EquipWeaponCPP.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../../EnemyBase.h"

EBTNodeResult::Type UBTT_EquipWeaponCPP::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

    // EnemyBase로 캐스팅
    EnemyBase = Cast<AEnemyBase>(ControlledPawn);
    if (!EnemyBase)
    {
        return EBTNodeResult::Failed;
    }

    // EquipWeapon 함수 호출
    EnemyBase->EquipWeapon();

    // Weapon Equipped 이벤트 바인딩
    EnemyBase->OnWeaponEquipped.AddDynamic(this, &UBTT_EquipWeaponCPP::OnWeaponEquipped);

    // OwnerComp 저장
    CachedOwnerComp = &OwnerComp;

    return EBTNodeResult::InProgress; // 비동기 작업이 진행 중임
}

void UBTT_EquipWeaponCPP::OnWeaponEquipped()
{
    if (CachedOwnerComp)
    {
        FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);

        // 이벤트 바인딩 해제 (메모리 누수 방지)
        if (EnemyBase)
        {
            EnemyBase->OnWeaponEquipped.RemoveDynamic(this, &UBTT_EquipWeaponCPP::OnWeaponEquipped);
        }
    }

}
