// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemies/EnemyBase.h"
#include "GameFramework/Actor.h"
#include "../../DamageSystem/DataOfDamage.h"
#include "../../Attacks/AttackInfo.h"
#include "EnemyMelee.generated.h"

UCLASS()
class AEnemyMelee : public AEnemyBase
{
	GENERATED_BODY()

public:
	// Constructor
	AEnemyMelee();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;




public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Override any additional functionality or add new functions here

	//TakeDamage 같은경우 나중에 선언하자


	UFUNCTION(BlueprintCallable, Category = "Attack")
	void GroundSmashAttack(AActor* AttackTarget);

	UFUNCTION(BlueprintCallable, Category = "Attack")
	void ShortRangeAttack(AActor* AttackTarget);

	UFUNCTION(BlueprintCallable, Category = "Attack")
	void LongRangeAttack(AActor* AttackTarget);

	UFUNCTION(BlueprintCallable, Category = "Attack")
	void SpinningAttack(AActor* AttackTarget);


protected:

	//--------------------------------Attack Of Skills-------------------------------------



	//------------------------Equip and UnEquip Weapon--------------------------------------- 나중에 만드는걸로

	UFUNCTION(BlueprintCallable, Category = "Equip")
	void EquipWeapon();

	UFUNCTION(BlueprintCallable, Category = "Equip")
	void UnEquipWeapon();

	//----------------------------Block and Suscess------Block 이미 만들어져있음




	UFUNCTION(BlueprintCallable, Category = "Block")
	void BlockTrigger();


	//-----------------------------------------------






private:
	// Mesh의 회전값을 저장하기 위한 변수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	FRotator MeshRotationBeforeSpin;




};
