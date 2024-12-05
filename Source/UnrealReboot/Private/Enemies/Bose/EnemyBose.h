// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemies/EnemyBase.h"
#include "GameFramework/Actor.h"
#include "Enemies/Bose/DataOfBoss.h"
#include "DamageSystem/DamageSystem.h"
#include "Attacks/CP_Attacks.h"
#include "EnemyBose.generated.h"

UCLASS()
class AEnemyBoss : public AEnemyBase
{
	GENERATED_BODY()

public:
	// Constructor
	AEnemyBoss();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;




	//--------------------------------Teleport------------------
	UFUNCTION()
	void Teleport(FVector Location, AActor* AttackTarget);

	UFUNCTION()
	void TeleportEnd();

//--------------------------------------------------------AttacksOfBoss
	UFUNCTION()
	void AttackCombo1(AActor* AttackTarget);

	UFUNCTION()
	void AttackCombo2(AActor* AttackTarget);

	UFUNCTION()
	void JumpAttack(AActor* AttackTarget);

	UFUNCTION()
	void AOETrail(AActor* AttackTarget);

	UFUNCTION()
	void GroundSmash(AActor* AttackTarget);

	UFUNCTION()
	void MissileLaunch(AActor* AttackTarget);

//--------------------------------------------Components

	//UPROPERTY()
	//UCP_Attacks* AttackComponent;//<- 포인터 안쓰면 가비지 컬렉터 오류 난다. 여기 말고 protect로 base 에 쓰는게?

	//UPROPERTY()
	//UDamageSystem* DamageSystemComponent;

	//------------------------------------------Montage
	UPROPERTY()
	UAnimMontage* Combo1Montage;

	UPROPERTY()
	UAnimMontage* Combo2Montage;


	//-----------------------------------------MontageHelpFunction

	UFUNCTION()
	void OnInterrupted();

	UFUNCTION()
	void OnMontageCompleted(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnNotifyBeginReceived_Combo2(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);


	//---------------------------------------------------MeshComponent

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	USkeletalMeshComponent* MeshComponent;



private:
	AActor* currentAttackTarget;

};
