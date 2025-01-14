// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_UnequipWeaponCPP.generated.h"

/**
 * 
 */
UCLASS()
class UBTT_UnequipWeaponCPP : public UBTTaskNode
{
	GENERATED_BODY()


protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;


	UBehaviorTreeComponent* CachedOwnerComp;

	class AEnemyBase* EnemyBase;
	void OnFinishOnSheath();
	
};
