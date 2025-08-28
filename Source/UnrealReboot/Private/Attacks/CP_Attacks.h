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

	// 이 몽타주가 반응해야 할 노티파이 이름. UPROPERTY로 안전하게 관리합니다.
	UPROPERTY()
	FName NotifyName;

	// 노티파이가 울렸을 때 실행될 실제 로직.
	// TFunction은 UPROPERTY가 될 수 없습니다.
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


	//------------------------------------------------MontageFunction---------- 삭제대상


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

	//----------------------------------------------------------------------------------------삭제대상

protected:
	//for Attack Info Matching
	//Protect 에서 이걸 사용할까? 모르겠네




	// 람다를 사용함으로써, 이제는 더이상 필요하지 않은 변수들
	FAttackInfo CurrentAttackInfo;
	FDamageInfo CurrentDamageInfo;
	FDamageInfo AOECachedDamageInfo;

	float CurrentRadius;
	float CurrentLength;


	//-----------------------------------------------------------------------------------------삭제대상





	USkeletalMeshComponent* CurrentMeshComponent;//Current Mesh 임시저장소 but 임시로 써둔거고 나중에 Mapping 을 통해서 바꿔줄것




private:
	


	//----------------------------------------------------------------------------------------삭제대상
	AActor* CurrentAttackTarget;

	ACharacter* CharacterRef;// 캐릭터 레퍼런스 받아오기 위해 선언

	//----------------------------------------------------------------------------------------삭제대상

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Projectile", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AProjectileBase> ProjectileSpawnClass; //Projectilebullet 클래스


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Projectile", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AAOEBase> AOEClass; //AOE 클래스
	UPROPERTY()
	TArray<AProjectileBase*> ActiveProjectiles;

	//----------------------------------------------------------------------------------------New! 20250804


	// Notify 이름을 상수로 관리하여 오타를 방지하고 중앙에서 관리
	const FName SlashNotifyName = FName("Slash");
	const FName SmashNotifyName = FName("Smash");
	const FName JumpNotifyName = FName("Jump");
	const FName FireNotifyName = FName("Fire");
	const FName AOESlashNotifyName = FName("AOESlash");

	//----------------------------------------------------------------------------------------New!
	  //모든 몽타주 재생 로직을 처리하는 단일 내부 함수
	void ExecuteAttack_Internal(UAnimMontage* MontageToPlay, const FName& NotifyName, TFunction<void(FName)> NotifyCallback);

	//  현재 재생 중인 몽타주와 콜백 정보를 1:1로 매핑하는 TMap
	TMap<UAnimMontage*, FAttackMontageInfo> ActiveMontageInfoMap;

protected:
	//공통 델리게이트 핸들러
	UFUNCTION()
	void HandleMontageNotify(FName InNotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	
};
