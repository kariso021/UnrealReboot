// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_DefaultAttackCPP.generated.h"

/**
 * 
 */
UCLASS()
class UBTT_DefaultAttackCPP : public UBTTaskNode
{
	GENERATED_BODY()


public:
	UBTT_DefaultAttackCPP();

protected:

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UFUNCTION()
	void OnAttackFinished();


private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default", meta = (AllowPrivateAccess = "true", DisplayName = "Attack Target Key"))
	FBlackboardKeySelector AttackTargetKey;





	UBehaviorTreeComponent* CachedOwnerComp;

	class AEnemyBase* EnemyBase;

	
};
