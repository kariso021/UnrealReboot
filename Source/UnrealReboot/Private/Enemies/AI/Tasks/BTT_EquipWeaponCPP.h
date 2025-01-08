// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_EquipWeaponCPP.generated.h"

/**
 * 
 */
UCLASS()
class UBTT_EquipWeaponCPP : public UBTTaskNode
{
	GENERATED_BODY()
	
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:

	UFUNCTION()
	void OnWeaponEquipped();

	UBehaviorTreeComponent* CachedOwnerComp;
	class AEnemyBase* EnemyBase;
};
