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
    // �⺻����
    UFUNCTION(BlueprintCallable, Category = "Abilities|Attack")
    void BasicAttack();

	// Gsmash ����
    UFUNCTION(BlueprintCallable, Category = "Abilities|Attack")
    void GroundSmashAttack();

    // ���� ����
    UFUNCTION(BlueprintCallable, Category = "Abilities|Attack")
    void BarrageAttack(AActor* AttackTarget);


    //================================================================
    // 2. �ڷ���Ʈ (Teleport)
    //================================================================

    // ���� ��ġ�� �ڷ���Ʈ
    UFUNCTION(BlueprintCallable, Category = "Abilities|Movement")
    void Teleport(const FVector& TargetLocation);

    // �ڷ���Ʈ ������ ȣ��� �̺�Ʈ
    UFUNCTION(BlueprintCallable, Category = "Abilities|Movement")
    void TeleportEnd();


    //-----------------------------------------
    // ġ��

    UFUNCTION(BlueprintCallable, Category = "Abilities|Heal")
    void StartHealOverTime(float HealAmountPerTick, float TickInterval, float Duration);

    /** �������� ġ�� ȿ���� �ߴ��մϴ�. */
    UFUNCTION(BlueprintCallable, Category = "Abilities|Heal")
    void StopHealOverTime();

private:


	bool bIsTeleporting = false; // Teleportation ���¸� ��Ÿ���� ����
	//subclass of AOEBase
	UPROPERTY(EditAnywhere, Category = "HealAOE")
	AAOEBase* HealAOE;


protected:

    // �ڷ���Ʈ �� ���� ǥ�õ� ����Ʈ (�����Ϳ��� ����)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
    UParticleSystem* TeleportBodyEffect;

    // �ڷ���Ʈ �� ������ ���� ����Ʈ (�����Ϳ��� ����)
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
