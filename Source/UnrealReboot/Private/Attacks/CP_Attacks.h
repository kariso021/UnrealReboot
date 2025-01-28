// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttackInfo.h"
#include "DamageSystem/DamageableInterface.h"
#include <Perception/AIPerceptionSystem.h>
#include <Perception/AISense_Damage.h>
#include <Perception/AISense.h>
#include "DamageSystem/DamageInfoMapping.h" //Projectile�� DamageInfo�� ������ѢB�� ���� ���̺귯�� ����
#include "DamageSystem/AOEDamageInfoMapping.h"
#include "../Projectile/ProjectileBase.h"

#include "CP_Attacks.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackEnded);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UCP_Attacks : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCP_Attacks();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	TArray<AActor*> DamageAllNonTeamMembers(const TArray<FHitResult>& Hits, FDamageInfo& DamageInfo);

	UFUNCTION()
	AActor* DamageFirstNonTeamMembers(const TArray<FHitResult>& Hits, FDamageInfo& DamageInfo);

	UFUNCTION()
	USkeletalMeshComponent* AttackBase(AActor* AttackTarget);

	UFUNCTION()
	FVector CalculateFutureActorLocation(AActor* Actor, float Time);

	//------------------------------------------------------Attacks-----------------------------------------------------
	
	UFUNCTION()
	void FireBullet(FVector TraceStart,FVector TraceEnd,FDamageInfo &DamageInfo);

	UFUNCTION()
	void MagicSpell(FTransform& SpawnTransform, AActor* Target, FDamageInfo& DamageInfo, TSubclassOf<AProjectileBase> ClassofSpawn);

	UFUNCTION()
	void AOEDamage(float Radius, FDamageInfo& DamageInfo);

	UFUNCTION()
	void AOEDamageActor(AActor* Actor);

	UFUNCTION()
	void SphereTraceDamage(float Length, float Radius, FDamageInfo& DamageInfo);

	UFUNCTION()
	void JumpToAttackTarget(AActor* AttackTarget);

	UFUNCTION()
	void GroundSmash(FAttackInfo& AttackInfo, float Radius);

	UFUNCTION()
	void PrimaryMeleeAttack(FAttackInfo& AttackInfo, float Radius, float Length);

	UFUNCTION()
	void LongRangeMeleeAttack(FAttackInfo& AttackInfo);

	UFUNCTION()
	void SpinningMeleeAttack(FAttackInfo& AttackInfo);

	UFUNCTION()
	void BasicMageSpell(FAttackInfo& AttackInfo);

	UFUNCTION()
	void BarrageMagicSpell(FAttackInfo& AttackInfo);

	UFUNCTION()
	void OnProjectileHit(AActor* OtherActor, const FHitResult& Hit);

	UFUNCTION()
	void OnLand(const FHitResult& Hit);



	//---------------------------------------------------AI Perception----------------------------------------------

	UFUNCTION()
	void ReportDamageEvent(UObject* WorldContextObject, AActor* DamagedActor, AActor* InstigatorActor, float DamageAmount, FVector EventLocation, FVector HitLocation);




	//-------------------------------------------------EndofAttack----------
	UPROPERTY(BlueprintAssignable, Category = "EndofAttack")
	FOnAttackEnded OnAttackEnd;


	//------------------------------------------------MontageFunction----------


	UFUNCTION()
	void OnMontageCompleted(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnMontageBlendedOut(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnNotifyBeginReceived_GroundSmash(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

	UFUNCTION()
	void OnNotifyBeginReceived_PrimaryMeleeAttack(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

	UFUNCTION()
	void OnNotifyBeginReceived_LongRangeMeleeAttack(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

	UFUNCTION()
	void OnNotifyBeginReceived_SpinningMeleeAttack(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

	UFUNCTION()
	void OnNotifyBeginReceived_BasicMageSpell(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

	UFUNCTION()
	void OnNotifyBeginReceived_BarrageMagicSpell(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);



	UFUNCTION()
	void OnInterrupted();

	UFUNCTION()
	void OnMontageInterrupted(UAnimMontage* Montage, bool bInterrupted);

protected:
	//for Attack Info Matching
	//Protect ���� �̰� ����ұ�? �𸣰ڳ�
	FAttackInfo CurrentAttackInfo;
	FDamageInfo CurrentDamageInfo;


	float CurrentRadius;
	float CurrentLength;
	USkeletalMeshComponent* CurrentMeshComponent;//Current Mesh �ӽ������ but �ӽ÷� ��аŰ� ���߿� Mapping �� ���ؼ� �ٲ��ٰ�




private:
	TArray<AActor*> ActorsDamagedSoFar;

	AActor* CurrentAttackTarget;

	ACharacter* CharacterRef;// ĳ���� ���۷��� �޾ƿ��� ���� ����

	TMap<AProjectileBase*, FDamageInfo> ProjectileDamageMap;
	TOptional<FAOEDamageInfoMapping> AOEDamageInfoMapping;

	UPROPERTY()
	TArray<AProjectileBase*> ActiveProjectiles;
		
};
