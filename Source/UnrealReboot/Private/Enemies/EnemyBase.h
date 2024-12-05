// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemies/EnemyAIInterface.h"
#include "DamageSystem/DamageableInterface.h"
#include <GameFramework/CharacterMovementComponent.h>
#include "DamageSystem/WD_HealthBar.h"
#include "Attacks/CP_Attacks.h"
#include <Components/WidgetComponent.h>
#include "DamageSystem/DamageSystem.h"
#include <BehaviorTree/BehaviorTree.h>
#include <Kismet/GameplayStatics.h>
#include <Kismet/GameplayStaticsTypes.h>
#include "EnemyBase.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackEndDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponEquippedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponUnEquippedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBlockEndedDelegate);




UCLASS()
class AEnemyBase : public ACharacter,public IDamageableInterface,public IEnemyAIInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

//----------------------Delegate

    UPROPERTY()
    FOnAttackEndDelegate OnAttackEnd;

    UPROPERTY()
    FOnWeaponEquippedDelegate OnWeaponEquipped;

    UPROPERTY()
    FOnWeaponUnEquippedDelegate OnWeaponUnEquipped;

    UPROPERTY()
    FOnBlockEndedDelegate OnBlockEnded;

    //빨간줄 일어나는건 프로젝트 재생성하면 없어진다고 하네



//------------------------------Interface 전부 virtual 로

     // IDamageableInterface required methods
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

    // IEnemyAIInterface required methods
    virtual float SetMovementSpeed(EM_MovementSpeed Speed) override;
    virtual FRangeofState GetIdealRange() override;
    virtual void EquipWeapon() override;
    virtual void UnequipWeapon() override;
    virtual void Attack(AActor* AttackTarget) override;
    virtual void JumpToDestination(FVector Destination) override;
    virtual bool AttackStart(AActor* AttackTarget, int TokenNeeded) override;
    virtual void AttackEnd(AActor* AttackTarget) override;
    virtual void StoreAttackToken(AActor* AttackTarget, int Amount) override;
    virtual APatrolRoute* GetPatrolRoute() override;


 //--------------------------------------------------Public Source------------------------
    //like Bt, or Team Number
    UPROPERTY()
    int TeamNumber;


    //---------------------------------------------Events------------------------------------

    UFUNCTION()
    void Die();



    //--------------------------------------------------Block Event
    UFUNCTION()
    void StartBlock();

    UFUNCTION()
    void HitResponse(EM_DamageResponse damageresponse,AActor* DamageCauser);


    UFUNCTION()
    void EndBlock();

    UFUNCTION()
    void HandleBlockedEvent(bool bCanBeParried, AActor* DamageCauser);
    //여기서 DAMAGESYSTEM Component 에서 생성한 Delegate 를 이용하는것

    //Public으로 무조건 놔야겠다. 다른곳에서 참조를 할수가 없네
    UPROPERTY(EditDefaultsOnly, Category = "AI")
    UBehaviorTree* BehaviorTree;

    UPROPERTY()
    APatrolRoute* PatrolRoute;//AActor 로 해도 됨

    
 






protected:
    UPROPERTY()
    UWidgetComponent* HealthBarComp;

    UPROPERTY()
    UCP_Attacks* AttackComponent;//<- 포인터 안쓰면 가비지 컬렉터 오류 난다. 여기 말고 protect로 base 에 쓰는게?

    UPROPERTY()
    UDamageSystem* DamageSystemComponent;

    UPROPERTY(EditAnywhere)
    UAnimMontage* BlockMontage;


  

 








private:
    bool IsWieldingWeapon;
    AActor* WeaponActor;
    UAnimMontage* HitReactionMontage;
    bool Attacking;
    TMap<AActor*, int32> AttackTokens;
    int TokensUsedInCurrentAttack;
    UObject* AttackIndicatorWidget;
    UAnimMontage* StaggerMontage;
    FTimerHandle HoldBlockTimer;
    float BlockChance;
    bool WhenDied;
};
