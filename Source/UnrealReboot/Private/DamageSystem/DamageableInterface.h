// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DataOfDamage.h"
#include "DamageableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDamageableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class IDamageableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION()
	virtual float GetCurrentHealth();

	UFUNCTION()
	virtual void SetIsInvincible(bool set);

	UFUNCTION()
	virtual float GetMaxHealth();

	UFUNCTION()
	virtual float Heal(float Amount);

	UFUNCTION()
	virtual bool IsDead();

	UFUNCTION()
	virtual bool TakeDamage(FDamageInfo& DamageInfo, AActor* DamageCauser);

	UFUNCTION()
	virtual bool IsAttacking();

	UFUNCTION()
	virtual bool ReserveAttackToken(int Amount);

	UFUNCTION()
	virtual void ReturnAttackToken(int Amount);

	UFUNCTION()
	virtual int GetTeamNumber();

	UFUNCTION()
	virtual void SetIsInterruptible(bool Value);
};
