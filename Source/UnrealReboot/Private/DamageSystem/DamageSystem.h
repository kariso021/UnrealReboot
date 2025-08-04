// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DataOfDamage.h"
#include "DamageSystem.generated.h"


UENUM(BlueprintType)
enum class EDamageResult : uint8
{
	Block        UMETA(DisplayName = "Blocked"),
	Damage       UMETA(DisplayName = "Damaged"),
	NoDamage     UMETA(DisplayName = "NoDamage"),
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBlockedDelegate, bool, bCanBeParried, AActor*, DamageCauser);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDamageResponseDelegate, EM_DamageResponse, DamageResponse, AActor*, DamageCauser);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathDelegate);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UDamageSystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDamageSystem();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//Custom 함수들 모음

	UFUNCTION()
	float HealCPP(float Amount);
	UFUNCTION()
	bool TakeDamageCPP(const FDamageInfo& DamageInfo, AActor* DamagedActor);
	UFUNCTION()
	bool ReserveAttackTokenCPP(int Amount);
	UFUNCTION()
	void ReturnAttackTokenCPP(int Amount);

	UFUNCTION()
	void SetAttackTokenCount(int token);

	UFUNCTION()
	EDamageResult CanBeDamaged(bool IsInvincible, bool CanBeBlo);

	UFUNCTION()
	bool GetIsDead();

	UFUNCTION()
	float GetMaxHP_FromComponent();

	UFUNCTION()
	float GetCurrentHP_FromComponent();

	UFUNCTION()
	void SetIsInterruptible_FromComponent(bool value);

	UFUNCTION()
	void SetIsBlocking(bool Input);




	UPROPERTY(BlueprintAssignable, Category = "Combat")
	FOnBlockedDelegate OnBlocked;

	UPROPERTY(BlueprintAssignable, Category = "Combat")
	FOnDamageResponseDelegate OnDamageResponse;

	UPROPERTY(BlueprintAssignable, Category = "Combat")
	FOnDeathDelegate OnDeath;

	UFUNCTION()
	void SetInvincible(bool invin);

	UFUNCTION()
	void SetMaxhealth(float Max);

	UFUNCTION()
	void SetHealth(float current);



private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health", meta = (AllowPrivateAccess = "true"))
	float HealthCPP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health", meta = (AllowPrivateAccess = "true"))
	float MaxHealthCPP;
	bool IsInvincibleCPP;
	bool IsDeadCPP;
	bool IsInterruptibleCPP;
	bool IsBlockingCPP;
	int AttackTokensCountCPP;
		
};



