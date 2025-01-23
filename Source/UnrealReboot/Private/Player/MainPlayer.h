// Fill out your copyright notice in the Description page of Project Settings.
// 꺽쇠표시는 언리얼 내의 자체엔진, 그외 ""표시는 내가 작성한 스크립가 규칙임

#pragma once

#include "CoreMinimal.h"
#include <GameFramework/Character.h>
#include <Components/SkinnedMeshComponent.h>
#include "Attacks/CP_Attacks.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include <Components/ArrowComponent.h>//Debug용 Arrow Componnent
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
	AActor* EquipWeapon(TSubclassOf<AActor> WeaponBlueprint, FName SocketName);

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
	USoundBase* DodgeSound; // 재생할 소리


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

	//Shooting 관련 변수
	bool Canshoot;
	int remainingbullet = 20; //초기시작탄 20발로 그냥 설정해두기
	int maxBullet = 30;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player State", meta = (AllowPrivateAccess = "true"))
	EPlayerStance Stance;


	float magicWalkSpeed=200;
	float defaultWalkSpeed=750;
	float meleeWalkSpeed=430;
	bool canMove;
	bool attacking;
	FVector aimBoomOffset;
	FVector defaultBoomOffset;

	//---------------------------Weapon관련 변수
	UPROPERTY()
	AActor* WeaponActor;

	TArray<AActor*> EquippedWeapon;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> WeaponSwordClass; // 무기 Blueprint 클래스


	//Murdog 캐릭터에 이미 Gun이 딸려있어서 총 클래스는 배제하도록 함
	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	//TSubclassOf<AActor> WeaponGunClass; //

	//------------------------------------------------------------



	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Team", meta = (AllowPrivateAccess = "true"))
	uint8 teamNumber;

	
	bool isWithinResumeComboWi;
	float swordDashFloat;

	bool readyToDodge;
	bool bHasDodged;
	bool bIsInvincible; 
	bool bhasBlocked;
	bool IsWithinParryCombo;

	FTimerHandle DelayTimerHandle;


	//----------------------------------------Input는 여기에다가

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

	

	// Movement Handlers


/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void Dodge(const FInputActionValue& Value);

	void Slot1(const FInputActionValue& Value);

	void Slot2(const FInputActionValue& Value);

	void Slot3(const FInputActionValue& Value);
	



};
