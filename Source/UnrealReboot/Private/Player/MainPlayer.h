// Fill out your copyright notice in the Description page of Project Settings.
// ����ǥ�ô� �𸮾� ���� ��ü����, �׿� ""ǥ�ô� ���� �ۼ��� ��ũ���� ��Ģ��

#pragma once

#include "CoreMinimal.h"
#include <GameFramework/Character.h>
#include <Components/SkinnedMeshComponent.h>
#include "Attacks/CP_Attacks.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include <Components/ArrowComponent.h>//Debug�� Arrow Componnent
#include "DamageSystem/DamageSystem.h"
#include "DamageSystem/DamageableInterface.h"
#include "EnhancedInputSubsystems.h"
#include "EnumPlayerStance.h"
#include "EnhancedInputComponent.h" //Inputs
#include "InputAction.h"
#include "InputMappingContext.h"
#include "DamageSystem/DamageSystem.h"//DamageSystem
#include "Weapon/WeaponBase.h"
#include "MainPlayer.generated.h"

UCLASS()
class AMainPlayer : public ACharacter,public IDamageableInterface
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;




public:
	// Sets default values for this character's properties
	AMainPlayer();

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


//----------------------------------Property--------------------------------------------------------------------------------------------






	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	UArrowComponent* ForwardArrow;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UDamageSystem* DamageSystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCP_Attacks* AttacksComponent;


	//-------------------------------------------------Function------------------------------------------------------------
	UFUNCTION()
	void ChangeStance(EPlayerStance S);

	UFUNCTION()
	AWeaponBase* EquipWeapon(TSubclassOf<AWeaponBase> WeaponClass, FName SocketName);

	UFUNCTION()
	void UnEquipAllWeapon();

	UFUNCTION()
	bool CheckCanShoot();
	//------------------------------------for Dodge


	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystem* DodgeEffect1;//teleport start

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystem* DodgeEffect2;//teleport tail

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystem* DodgeEffect3;//teleport end

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundBase* DodgeSound; // ����� �Ҹ�


	//--------------------------------Dodge-----------------------------------------------
	UFUNCTION()
	void DodgeDirection(float distance);

	UFUNCTION()
	void DodgeToLocation(const FVector& Location);

	UFUNCTION()
	void ResetDodge();

	UFUNCTION()
	void WhenActionAfterDodgeEnd();

	UFUNCTION()
	void WhenBeforeReset();



	//-----------------------------------------------Function for Delegate--------------------------------------------------------

	UFUNCTION()
	void OnDeath();









	//---------------------------------------------InterfaceFuntion------------------------------------------------
	virtual float GetCurrentHealth() override;
	virtual float GetMaxHealth() override;
	virtual float Heal(float Amount) override;
	virtual bool IsDead() override;
	virtual bool TakeDamage(FDamageInfo& DamageInfo, AActor* DamageCauser) override;
	virtual bool IsAttacking() override;
	virtual bool ReserveAttackToken(int Amount) override;
	virtual void ReturnAttackToken(int Amount) override;
	virtual int GetTeamNumber() override;
	virtual void SetIsInterruptible(bool Value) override;
	virtual void SetIsInvincible(bool value) override;

	//----------------------------------------------Function of Combat-----------------------------------------------
	UFUNCTION()
	void StartBlock();

	UFUNCTION()
	void EndBlock();

	UFUNCTION()
	void ResetBlock();

	UFUNCTION()
	void MeleeAttack();

	UFUNCTION()
	void ResetMeleeAttack();

	UFUNCTION()
	void Reload();

	//-----------------------------------------------------Source of Montage---------------------------

	UAnimMontage* MontageBlock;



	//----------------------------------------------------------FunctionOf__MontageAnimation--------------

	UFUNCTION()
	void OnMontageCompleted_Block(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnNotifyBeginReceived_Block(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

	UFUNCTION()
	void OnNotifyEndReceived_Block(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

	UFUNCTION()
	void OnInterrupted_Block();











	//--------------------------------------------------StanceEventOfPlayer-----------------------------------------
	UFUNCTION()
	void UnarmedStance();
	UFUNCTION()
	void MeleeStance();
	UFUNCTION()
	void EnterRangedStance();


private:

	//Shooting ���� ����
	bool Canshoot;
	int remainingbullet = 20; //�ʱ����ź 20�߷� �׳� �����صα�
	int maxBullet = 30;


	TArray<AWeaponBase*> EquippedWeapon;
	EPlayerStance Stance;
	float magicWalkSpeed;
	float defaultWalkSpeed;
	bool canMove;
	bool attacking;
	FVector aimBoomOffset;
	FVector defaultBoomOffset;
	int teamNumber;
	float meleeWalkSpeed;
	bool isWithinResumeComboWi;
	float swordDashFloat;

	bool readyToDodge;
	bool bHasDodged;
	bool bIsInvincible; 
	bool bhasBlocked;
	bool IsWithinParryCombo;

	FTimerHandle DelayTimerHandle;


	//----------------------------------------Input�� ���⿡�ٰ�

protected:


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* InputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* DodgeAction;

	

	// Movement Handlers


/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void Dodge(const FInputActionValue& Value);
	



};