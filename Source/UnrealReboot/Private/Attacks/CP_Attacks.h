// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttackInfo.h"
#include "DamageSystem/DamageableInterface.h"
#include <Perception/AIPerceptionSystem.h>
#include <Perception/AISense_Damage.h>
#include <Perception/AISense.h>
#include "../Projectile/ProjectileBase.h"
#include "../AOE/AOEBase.h"

#include "CP_Attacks.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackEnded);
USTRUCT()
struct FAttackMontageInfo
{
	GENERATED_USTRUCT_BODY()

	// �� ��Ÿ�ְ� �����ؾ� �� ��Ƽ���� �̸�. UPROPERTY�� �����ϰ� �����մϴ�.
	UPROPERTY()
	FName NotifyName;

	// ��Ƽ���̰� ����� �� ����� ���� ����.
	// TFunction�� UPROPERTY�� �� �� �����ϴ�.
	TFunction<void(FName)> NotifyCallback;
};





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
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	TArray<AActor*> DamageAllNonTeamMembers(const TArray<FHitResult>& Hits, const FDamageInfo& DamageInfo);

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
	void MagicSpell(FTransform& SpawnTransform, AActor* Target, FDamageInfo& DamageInfo);

	UFUNCTION()
	void AOEDamage(float Radius, const FDamageInfo& DamageInfo);

	UFUNCTION()
	void AOEDamageActor(AActor* Actor);

	UFUNCTION()
	void SphereTraceDamage(float Length, float Radius, const FDamageInfo& DamageInfo);

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


	//------------------------------------------------MontageFunction---------- �������


	UFUNCTION()
	void OnMontageCompleted_UseInterrupted(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnMontageCompleted_NotUseInterrupted(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnMontageBlendedOut(UAnimMontage* Montage, bool bInterrupted);


	UFUNCTION()
	void OnInterrupted(bool Interrupted);

	UFUNCTION()
	void OnMontageInterrupted(UAnimMontage* Montage, bool bInterrupted);

	//----------------------------------------------------------------------------------------�������

protected:
	//for Attack Info Matching
	//Protect ���� �̰� ����ұ�? �𸣰ڳ�




	// ���ٸ� ��������ν�, ������ ���̻� �ʿ����� ���� ������
	FAttackInfo CurrentAttackInfo;
	FDamageInfo CurrentDamageInfo;
	FDamageInfo AOECachedDamageInfo;

	float CurrentRadius;
	float CurrentLength;


	//-----------------------------------------------------------------------------------------�������





	USkeletalMeshComponent* CurrentMeshComponent;//Current Mesh �ӽ������ but �ӽ÷� ��аŰ� ���߿� Mapping �� ���ؼ� �ٲ��ٰ�




private:
	


	//----------------------------------------------------------------------------------------�������
	AActor* CurrentAttackTarget;

	ACharacter* CharacterRef;// ĳ���� ���۷��� �޾ƿ��� ���� ����

	//----------------------------------------------------------------------------------------�������

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Projectile", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AProjectileBase> ProjectileSpawnClass; //Projectilebullet Ŭ����


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Projectile", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AAOEBase> AOEClass; //AOE Ŭ����
	UPROPERTY()
	TArray<AProjectileBase*> ActiveProjectiles;

	//----------------------------------------------------------------------------------------New! 20250804


	// Notify �̸��� ����� �����Ͽ� ��Ÿ�� �����ϰ� �߾ӿ��� ����
	const FName SlashNotifyName = FName("Slash");
	const FName SmashNotifyName = FName("Smash");
	const FName JumpNotifyName = FName("Jump");
	const FName FireNotifyName = FName("Fire");
	const FName AOESlashNotifyName = FName("AOESlash");

	//----------------------------------------------------------------------------------------New!
	  //��� ��Ÿ�� ��� ������ ó���ϴ� ���� ���� �Լ�
	void ExecuteAttack_Internal(UAnimMontage* MontageToPlay, const FName& NotifyName, TFunction<void(FName)> NotifyCallback);

	//  ���� ��� ���� ��Ÿ�ֿ� �ݹ� ������ 1:1�� �����ϴ� TMap
	TMap<UAnimMontage*, FAttackMontageInfo> ActiveMontageInfoMap;

protected:
	//���� ��������Ʈ �ڵ鷯
	UFUNCTION()
	void HandleMontageNotify(FName InNotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	
};
