// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
//AIC_EnemyBaseCPP
#include "UObject/Class.h"
#include "Enemies/DataOfEnemies.h"
#include "CoreMinimal.h"
#include "Enemies/EnemyAIInterface.h"
#include "Perception/AISense_Sight.h"
#include <Perception/AISense_Hearing.h>
#include <Perception/AIPerceptionTypes.h>
#include <Perception/AISense_Damage.h>
#include "AIController.h"
#include "Components/ActorComponent.h"
#include <Perception/AIPerceptionComponent.h>
#include <BehaviorTree/BlackboardComponent.h>
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Damage.h"
#include "AIC_EnemyBaseCPP.generated.h"

class AEnemyBase;
/**
 * 
 */
UCLASS()
class AAIC_EnemyBaseCPP : public AAIController
{
	GENERATED_BODY()
public:


	AAIC_EnemyBaseCPP();

	//TransFormComponent �̹� Scene Component ���� ����

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UActorComponent* ActionsComp;


	//PathFollowing Component �̹� ����� ����

//---------------------------------------------------------------------------------Functions------
	UFUNCTION()
	void SetStateAsPassive();

	UFUNCTION()
	void SetStateAsAttacking(AActor* AttackTarget,bool UseLastKnownAttackTarget);

	// Can Sense Actor

	UFUNCTION()
	void HandleSensedSight(AActor* AttackTarget);

	UFUNCTION()
	void HandleSensedSound(FVector Location);

	UFUNCTION()
	void SetStateAsInvestigating(FVector Location);

	UFUNCTION()
	EM_AIState GetCurrentState();

	UFUNCTION()
	void HandleSensedDamage(AActor* actor);

	UFUNCTION()
	void SetStateAsDead();

	UFUNCTION()
	void SetStateAsFrozen();

	UFUNCTION()
	void HandleForgotActor(AActor* Actor);

	UFUNCTION()
	void HandleLostSight(AActor* Actor);

	UFUNCTION()
	void SetStateAsSeeking(FVector Location);

	UFUNCTION()
	bool OnSameTeam(AActor* OtherAcotr);

	UFUNCTION()
	FCheckSensedStimulus CanSenseActor(AActor* actor, EM_AISense sense);

//--------------------------------------------------Event

	UFUNCTION()
	void SeekAttackTarget();

	UFUNCTION()
	void CheckForgottenSeenActor();

	UFUNCTION()
	void HandleForgottenActor(AActor* actor);
	


	//���� EventGraph �� �ִ� ���븸 �־��ָ� �ɵ�.
	UPROPERTY()
	UBlackboardComponent* BlackboardComp;

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAIPerceptionComponent* AIPerceptionComponent;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UAISenseConfig_Sight* SightConfig;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UAISenseConfig_Hearing* HearingConfig;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UAISenseConfig_Damage* DamageSenseConfig;



	UFUNCTION()
	void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);


	UPROPERTY(EditAnywhere, Category = "AI|Blackboard")
	FName AttackTargetKeyName;
	UPROPERTY(EditAnywhere, Category = "AI|Blackboard")
	FName StateKeyName;
	UPROPERTY(EditAnywhere, Category = "AI|Blackboard")
	FName PointOfInterestKeyName;
	UPROPERTY(EditAnywhere, Category = "AI|Blackboard")
	FName AttackRadiusKeyName;
	UPROPERTY(EditAnywhere, Category = "AI|Blackboard")
	FName DefendRadiusKeyName;
	UPROPERTY(EditAnywhere, Category = "AI|Blackboard")
	FName DistanceToAttackTargetKeyName;


private:
	// Names
	void InitializeSightConfig();

	void InitializeHearingConfig();

	void InitializeDamageSenseConfig();



	bool MatchSenseType(const FAISenseID& StimulusType, EM_AISense SenseType) const;


	// Actor
	AActor* AttackTarget;
	TArray<AActor*> KnownSeenActors;

	// Timer Handles
	FTimerHandle CheckForgottenActorsTimer;
	FTimerHandle SeekAttackTargetTimer;

	// Float
	UPROPERTY(EditAnywhere, Category = "AI|Timers")
	float TimeToSeekAfterLosingSight =3.0f;


};
