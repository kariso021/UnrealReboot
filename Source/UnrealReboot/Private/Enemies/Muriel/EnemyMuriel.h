// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemies/EnemyBase.h"
#include "GameFramework/Actor.h"
#include "../../AOE/AOEBase.h"
#include "EnemyMuriel.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMurielAttackEnded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHealOverTimeEnd);


UCLASS()
class AEnemyMuriel : public AEnemyBase
{
	GENERATED_BODY()

public:
	// Constructor
	AEnemyMuriel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
	FOnMurielAttackEnded OnMurielAttackEnded;

	UPROPERTY()
	FOnHealOverTimeEnd OnHealOverTimeEnd;

protected:
    // 기본공격
    UFUNCTION(BlueprintCallable, Category = "Abilities|Attack")
    void BasicAttack();

	// Gsmash 공격
    UFUNCTION(BlueprintCallable, Category = "Abilities|Attack")
    void GroundSmashAttack();

    // 연발 공격
    UFUNCTION(BlueprintCallable, Category = "Abilities|Attack")
    void BarrageAttack(AActor* AttackTarget);


    //================================================================
    // 2. 텔레포트 (Teleport)
    //================================================================

    // 지정 위치로 텔레포트
    UFUNCTION(BlueprintCallable, Category = "Abilities|Movement")
    void Teleport(const FVector& TargetLocation);

    // 텔레포트 종류후 호출될 이벤트
    UFUNCTION(BlueprintCallable, Category = "Abilities|Movement")
    void TeleportEnd();


    //-----------------------------------------
    // 치유

    UFUNCTION(BlueprintCallable, Category = "Abilities|Heal")
    void StartHealOverTime(float HealAmountPerTick, float TickInterval, float Duration);

    /** 지속적인 치유 효과를 중단합니다. */
    UFUNCTION(BlueprintCallable, Category = "Abilities|Heal")
    void StopHealOverTime();

private:


	bool bIsTeleporting = false; // Teleportation 상태를 나타내는 변수
	//subclass of AOEBase
	UPROPERTY(EditAnywhere, Category = "HealAOE")
	AAOEBase* HealAOE;


protected:

    // 텔레포트 시 몸에 표시될 이펙트 (에디터에서 설정)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
    UParticleSystem* TeleportBodyEffect;

    // 텔레포트 시 남겨질 흔적 이펙트 (에디터에서 설정)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
    UParticleSystem* TeleportTrailEffect;

    UPROPERTY()
    TObjectPtr<UParticleSystemComponent> SpawnedBodyEffectComponent;

    UPROPERTY()
    TObjectPtr<UParticleSystemComponent> SpawnedTrailEffectComponent;

private:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
    UAnimMontage* BarrageMontage;

};
