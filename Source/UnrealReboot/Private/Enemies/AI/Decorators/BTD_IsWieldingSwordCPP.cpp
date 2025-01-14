// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/AI/Decorators/BTD_IsWieldingSwordCPP.h"
#include "AIController.h"
#include"../../EnemyBase.h"

bool UBTD_IsWieldingSwordCPP::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    // AIController 및 Controlled Pawn 가져오기
    AAIController* AIController = OwnerComp.GetAIOwner();
    APawn* ControlledPawn = AIController ? AIController->GetPawn() : nullptr;

    if (!ControlledPawn)
    {
        return false; // Pawn이 없으면 조건 실패
    }

    // ControlledPawn을 BP_Enemy_Base로 캐스팅
    AEnemyBase* EnemyBase = Cast<AEnemyBase>(ControlledPawn);
    if (!EnemyBase)
    {
        return false; // 캐스팅 실패 시 조건 실패
    }

    // Is Wielding Weapon 확인
    return EnemyBase->GetisWieldingWeapon();
}
