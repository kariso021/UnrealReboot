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

	//TakeDamage ������� ���߿� ��������


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



	//------------------------Equip and UnEquip Weapon--------------------------------------- ���߿� ����°ɷ�

	UFUNCTION(BlueprintCallable, Category = "Equip")
	void EquipWeapon();

	UFUNCTION(BlueprintCallable, Category = "Equip")
	void UnEquipWeapon();


	//-----------------------------------------------------------------------------------------



	//----------------------------Block and Suscess------Block �̹� �����������




	UFUNCTION(BlueprintCallable, Category = "Block")
	void BlockTrigger();


	//-----------------------------------------------






private:
	// Mesh�� ȸ������ �����ϱ� ���� ����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	FRotator MeshRotationBeforeSpin;

	// �������Ʈ���� ���� ������ �ִϸ��̼� ��Ÿ��
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* GroundSmashMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* SwordSlashMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* SwordJumpAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* SwordSpinningAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* UnSheathSwordMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* SheathSwordMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* SwordBlockMontage;


	//-------------------------------------------------------------------MontageFunction


	UFUNCTION()
	void UnSheath_MontageCompleted(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void Sheath_MontageCompleted(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnNotifyBeginReceived_EquipWeapon(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

	UFUNCTION()
	void OnNotifyBeginReceived_UnEquipWeapon(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);




};
