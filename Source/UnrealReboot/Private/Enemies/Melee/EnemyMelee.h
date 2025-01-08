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


protected:

	UFUNCTION(BlueprintCallable, Category = "Attack")
	void GroundSmashAttack(AActor* AttackTarget);

	UFUNCTION(BlueprintCallable, Category = "Attack")
	void ShortRangeAttack(AActor* AttackTarget);

	UFUNCTION(BlueprintCallable, Category = "Attack")
	void LongRangeAttack(AActor* AttackTarget);

	UFUNCTION(BlueprintCallable, Category = "Attack")
	void SpinningAttack(AActor* AttackTarget);


private:
	// Mesh의 회전값을 저장하기 위한 변수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	FRotator MeshRotationBeforeSpin;




};
