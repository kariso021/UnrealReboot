// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/AI/Tasks/BTT_EquipWeaponCPP.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../../EnemyBase.h"

EBTNodeResult::Type UBTT_EquipWeaponCPP::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

    // OwnerComp 저장
    CachedOwnerComp = &OwnerComp;

    // AIController 가져오기
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to get AIController."));
        return EBTNodeResult::Failed;
    }

    // Controlled Pawn 가져오기
    APawn* ControlledPawn = AIController->GetPawn();
    if (!ControlledPawn)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to get Controlled Pawn."));
        return EBTNodeResult::Failed;
    }

    // EnemyBase 캐스팅
    EnemyBase = Cast<AEnemyBase>(ControlledPawn);
    if (!EnemyBase)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to cast ControlledPawn to AEnemyBase."));
        return EBTNodeResult::Failed;
    }

    // Weapon Equipped 이벤트 바인딩 (먼저 수행)
    if (!EnemyBase->OnWeaponEquipped.IsAlreadyBound(this, &UBTT_EquipWeaponCPP::OnWeaponEquipped))
    {
        UE_LOG(LogTemp, Log, TEXT("Binding OnWeaponEquipped event."));
        EnemyBase->OnWeaponEquipped.AddDynamic(this, &UBTT_EquipWeaponCPP::OnWeaponEquipped);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("OnWeaponEquipped event is already bound."));
    }

    // 인터페이스 구현 확인 및 EquipWeapon 호출
    if (EnemyBase->GetClass()->ImplementsInterface(UEnemyAIInterface::StaticClass()))
    {
        UE_LOG(LogTemp, Log, TEXT("Calling EquipWeapon on EnemyBase: %s"), *EnemyBase->GetName());
        IEnemyAIInterface::Execute_EquipWeapon(EnemyBase);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("EnemyBase does not implement UEnemyAIInterface."));
        return EBTNodeResult::Failed;
    }

    return EBTNodeResult::InProgress; // 비동기 작업 진행 중
}

void UBTT_EquipWeaponCPP::OnWeaponEquipped()
{
    if (CachedOwnerComp)
    {
        UE_LOG(LogTemp, Log, TEXT("OnWeaponEquipped event triggered."));

        // 이벤트 바인딩 해제
        if (EnemyBase)
        {
            EnemyBase->OnWeaponEquipped.RemoveDynamic(this, &UBTT_EquipWeaponCPP::OnWeaponEquipped);
        }

        // FinishLatentTask 호출
        UE_LOG(LogTemp, Log, TEXT("Finishing latent task with Succeeded."));
        FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("CachedOwnerComp is null. Cannot finish task."));
    }

}
