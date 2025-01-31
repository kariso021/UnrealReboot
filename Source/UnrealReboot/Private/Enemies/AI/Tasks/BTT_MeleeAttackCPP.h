// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "../../Melee/DataOfMelee.h"
#include "BTT_MeleeAttackCPP.generated.h"

/**
 * 
 */
UCLASS()
class UBTT_MeleeAttackCPP : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTT_MeleeAttackCPP();


protected:

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UFUNCTION()
	void OnAttackFinished();


private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default", meta = (AllowPrivateAccess = "true", DisplayName = "Attack Target Key"))
	FBlackboardKeySelector AttackTargetKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default", meta = (AllowPrivateAccess = "true", DisplayName = "Attack Radius Key"))
	FBlackboardKeySelector AttackRadiusKey;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default", meta = (AllowPrivateAccess = "true", DisplayName = "TokenNeeded"))
	uint8 TokenNeeded;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default", meta = (AllowPrivateAccess = "true", DisplayName = "AttackName"))
	EM_Melee_Atttacks AttackName;
	

	UBehaviorTreeComponent* CachedOwnerComp;

	class AEnemyMelee* EnemyMelee;
	class AAIController* CachedAIController;
	
};
