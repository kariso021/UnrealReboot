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
#include "Animation/AnimMontage.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "DamageSystem/DamageSystem.h"//DamageSystem
#include "Weapon/WeaponBase.h"
#include "../Projectile/ProjectileBase.h"
#include <Components/TimelineComponent.h>

#include "MainPlayer.generated.h"

//���漱��
class UPlayer_HUD_CPP;


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
	AActor* EquipWeapon(TSubclassOf<AActor> WeaponBlueprint, FName SocketName);

	UFUNCTION()
	void UnEquipAllWeapon();

	UFUNCTION()
	bool CheckCanShoot();
	//------------------------------------for Dodge


	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystem* DodgeEffect_Start;//teleport start

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystem* DodgeEffect_Tail;//teleport tail

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystem* DodgeEffect_End;//teleport end

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
	void WhenBeforeResetDodge();



	//-----------------------------------------------Function for Delegate--------------------------------------------------------

	UFUNCTION()
	void OnDeath();


	//---------------------------------------------------------Display HUD

	UFUNCTION()
	void DisplayHUD();








	//---------------------------------------------InterfaceFuntion------------------------------------------------
	virtual float GetCurrentHealth_Implementation() override;
	virtual float GetMaxHealth_Implementation() override;
	virtual float Heal_Implementation(float Amount) override;
	virtual bool IsDead_Implementation() override;
	virtual bool TakeDamage_Implementation(FDamageInfo& DamageInfo, AActor* DamageCauser) override;
	virtual bool IsAttacking_Implementation() override;
	virtual bool ReserveAttackToken_Implementation(int Amount) override;
	virtual void ReturnAttackToken_Implementation(int Amount) override;
	virtual int GetTeamNumber_Implementation() override;
	virtual void SetIsInterruptible_Implementation(bool Value) override;
	virtual void SetIsInvincible_Implementation(bool value) override;

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
	void ShootRangeAttack();
	
	void PerformMeleeAttack();
	void PerformRangeAttack();

	UFUNCTION()
	void ResetMeleeAttack();

	UFUNCTION()
	void ResetRangeAttack();

	UFUNCTION()
	void Reload();

	//-----------------------------------------------------Source of Montage---------------------------

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* MontageBlock;

	// �������Ʈ���� ���� ������ �ִϸ��̼� ��Ÿ��
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* SwordComboMontage01;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ShootingMontage;




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


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player State", meta = (AllowPrivateAccess = "true"))
	EPlayerStance Stance;


	// �⺻ �� ���� �� ī�޶� ������
	FVector DefaultBoomOffset;
	FVector AimBoomOffset;


	float magicWalkSpeed=200;
	float defaultWalkSpeed=750;
	float meleeWalkSpeed=430;
	bool canMove;
	bool attacking;

	bool bMeleeAttackExcute;
	bool bRangeAttackExcute;


	FVector aimBoomOffset;
	FVector defaultBoomOffset;

	//---------------------------Weapon���� ����
	UPROPERTY()
	AActor* WeaponActor;

	TArray<AActor*> EquippedWeapon;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> WeaponSwordClass; // ���� Blueprint Ŭ����

	//---------------------------SpawnActor(Projectile ���� ����)


	//Murdog ĳ���Ϳ� �̹� Gun�� �����־ �� Ŭ������ �����ϵ��� ��
	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	//TSubclassOf<AActor> WeaponGunClass; //

	//------------------------------------------------------------



	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Team", meta = (AllowPrivateAccess = "true"))
	uint8 teamNumber;

	
	bool isWithinResumeComboWindow;
	float swordDashFloat;

	bool readyToDodge;
	bool bHasDodged;
	bool bIsInvincible; 
	bool bhasBlocked;
	bool IsWithinParryCombo;
	bool bCanResumeCombo;

	FTimerHandle DelayTimerHandle;

	//----------------------------------------------------Widget ����


	  /** HUD ���� Ŭ���� */
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> HUDWidgetClass;

	/** ������ HUD �ν��Ͻ� */
	UPROPERTY()
	UPlayer_HUD_CPP* HUDWidget;


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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Slot1Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Slot2Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Slot3Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AttackAction;

	

	// Movement Handlers


/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void DodgeTriggered(const FInputActionValue& Value);

	void DodgeCompleted(const FInputActionValue& Value);

	void Slot1(const FInputActionValue& Value);

	void Slot2(const FInputActionValue& Value);

	void Slot3(const FInputActionValue& Value);

	void AcceptAttack(const FInputActionValue& Value);


	//--------------------------MontageFunction
	UFUNCTION()
	void OnNotifyBeginReceived_SwordCombo1(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

	UFUNCTION()
	void OnNotifyEndReceived_SwordCombo1(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

	UFUNCTION()
	void OnMontageCompleted_SwordCombo(UAnimMontage* Montage, bool bInterrupted);

	void OnInterrupted_SwordCombo();


	UFUNCTION()
	void OnNotifyBeginReceived_RangeShooting(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

	UFUNCTION()
	void OnMontageCompleted_RangeShooting(UAnimMontage* Montage, bool bInterrupted);

	void OnInterrupted_RangeShooting();


	// Ÿ�Ӷ��� (TimelineComponent)
	UPROPERTY()
	UTimelineComponent* AimTimeline;

	void StartAiming();
	void StopAiming();

	UPROPERTY(EditAnywhere, Category = "Camera")
	UCurveFloat* AimCurve;

	// Timeline ������Ʈ �Լ�
	UFUNCTION()
	void UpdateCameraOffset(float Alpha);
	



};
