// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Components/BoxComponent.h>
#include <Components/ArrowComponent.h>
#include <GameFramework/ProjectileMovementComponent.h>
#include <Particles/ParticleSystem.h>
#include "../DamageSystem/DataOfDamage.h"
#include <Sound/SoundBase.h>
#include "ProjectileBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnProjectileImpact, AActor*, OtherActor, const FHitResult&, Hit);

UCLASS()
class AProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectileBase();

	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* BoxCollision;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UArrowComponent* Arrow;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UProjectileMovementComponent* ProjectileMovement;

	void InitializeProjectile(float InputSpeed, float InputGravity, bool InputIsHoming, AActor* InputTarget);




	//---------------------------Event Dispatcher----------------------------
	UPROPERTY(BlueprintAssignable, Category="Projectile")
    FOnProjectileImpact OnProjectileImpact;

	// Projectile settings
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Projectile Settings")
	float Speed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Projectile Settings")
	float Gravity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Projectile Settings")
	bool IsHoming;

	// Effects
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Effects")
	UParticleSystem* ImpactEffect;

	// Sounds
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sounds")
	USoundBase* ImpactSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sounds")
	USoundBase* SpawnSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Target")
	AActor* Target;

	
	//--------------------------------On hit ÇÔ¼ö »©µÐ°Å


	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	//----------------------------------------Events
	UFUNCTION()
	void RotateToTarget();

	//Visual Effect
	UFUNCTION()
	void SpawnImpactEffect(FVector Location);

	//Sound Effect

	UFUNCTION()
	void PlayImpactSound(FVector Location);

	UFUNCTION()
	void PlaySpawnSound();



	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	USceneComponent* HomingTargetComponent;
	float HomingAcceleration;
	FVector CurrentVelocity;

	void InitHoming(USceneComponent* TargetComponent);

	void UpdateHoming(float DeltaTime);

};
