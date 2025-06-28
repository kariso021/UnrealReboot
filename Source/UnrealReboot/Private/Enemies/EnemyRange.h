// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemies/EnemyBase.h"
#include "GameFramework/Actor.h"
#include "Weapon/WeaponBase.h"
#include "EnemyRange.generated.h"


UCLASS()
class AEnemyRange : public AEnemyBase
{
	GENERATED_BODY()

public:
	// Constructor
	AEnemyRange();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	float SetmovementSpeed(EM_MovementSpeed Speed);


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual float GetAttackRadius_Implementation() override;
	virtual float GetDefendRadius_Implementation() override;

private:

	// 기총소사 거리와 공격 거리
	float DefendRadius = 600.0f;
	float AttackRadius = 600.0f;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> WeaponClass; // 무기 Blueprint 클래스


	void EquipWeapon_Implementation() override;

	void Attack_Implementation(AActor* AttackTarget) override;


private:
	// tempAttackTarget
	AActor* tempAttackTarget;


	// Animation Montage
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* RifleFireMontage;




	//Aniamtion Sequence Function
	UFUNCTION()
	void OnNotifyBeginReceived_EquipWeapon(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

	UFUNCTION()
	void Fire_MontageCompleted(UAnimMontage* Montage, bool bInterrupted);


};
