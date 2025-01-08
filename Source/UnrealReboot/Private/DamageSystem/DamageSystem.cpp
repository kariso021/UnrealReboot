// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageSystem/DamageSystem.h"

// Sets default values for this component's properties
UDamageSystem::UDamageSystem()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.

	HealthCPP = 100.0f;
	MaxHealthCPP = 100.0f;

	IsInvincibleCPP = false;
	IsDeadCPP = false;
	IsInterruptibleCPP = true;
	IsBlockingCPP = false;

	AttackTokensCountCPP = 0;


	PrimaryComponentTick.bCanEverTick = true;






	// ...
}


// Called when the game starts
void UDamageSystem::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UDamageSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

float UDamageSystem::HealCPP(float Amount)
{
	if (!IsDeadCPP)
	{
		HealthCPP = FMath::Clamp((Amount + HealthCPP), 0, MaxHealthCPP);
	}
	return HealthCPP;
}

bool UDamageSystem::TakeDamageCPP(const FDamageInfo& DamageInfo, AActor* DamageCauser)
{
	if (CanBeDamaged(DamageInfo.ShouldDamageInvincible, DamageInfo.CanBeBlocked) == 0)
	{
		//Block Damage
		OnBlocked.Broadcast(DamageInfo.CanBeParried, DamageCauser);
		return false;

	}
	else if (CanBeDamaged(DamageInfo.ShouldDamageInvincible, DamageInfo.CanBeBlocked) == 1)
	{
		//Do Damage
		HealthCPP = HealthCPP - DamageInfo.Amount;
		if (HealthCPP <= 0)
		{
			IsDeadCPP = true;
			OnDeath.Broadcast();
			return true;//Was Damaged
		}
		else
		{
			if (IsInterruptibleCPP || DamageInfo.ShouldForceInterrupt)
			{
				OnDamageResponse.Broadcast(DamageInfo.DamageResponse, DamageCauser);
				return true;//Was Damaged

			}
			else
			{
				return true;//WasDamaged
			}
		}
	}
	else if (CanBeDamaged(DamageInfo.ShouldDamageInvincible, DamageInfo.CanBeBlocked) == 2)
	{
		//No Damage
		return false;
	}
	return false; //CanBe Damaged 함수에 오류가 있을때 False 반환 0,1,2 중 그 어떤값도 반환하지 못했을때
}

bool UDamageSystem::ReserveAttackTokenCPP(int Amount)
{
	if (AttackTokensCountCPP >= Amount)
	{
		AttackTokensCountCPP -= Amount;
		return true; // Success
	}

	return false; // Failure
}

void UDamageSystem::ReturnAttackTokenCPP(int Amount)
{
	AttackTokensCountCPP = AttackTokensCountCPP + Amount;
}

void UDamageSystem::SetAttackTokenCount(int token)
{
	AttackTokensCountCPP = token;
}

int UDamageSystem::CanBeDamaged(bool IsInvincible, bool CanBeBlocked)
{
	if ((!IsDeadCPP) && ((!IsInvincibleCPP) || IsInvincible))
	{
		if (IsBlockingCPP && CanBeBlocked)
		{
			return 0;//BlockDamage
		}
		else
		{
			return 1;//DoDamage
		}
	}
	else
		return 2;//No Damage
}

bool UDamageSystem::GetIsDead()
{
	return IsDeadCPP;
}

float UDamageSystem::GetMaxHP_FromComponent()
{
	return MaxHealthCPP;
}

float UDamageSystem::GetCurrentHP_FromComponent()
{
	return HealthCPP;
}

void UDamageSystem::SetIsInterruptible_FromComponent(bool value)
{
	IsInterruptibleCPP = true;
}

void UDamageSystem::SetIsBlocking(bool Input)
{
	IsBlockingCPP = Input;
}

void UDamageSystem::SetInvincible(bool invin)
{
	IsInvincibleCPP = invin;
}

void UDamageSystem::SetMaxhealth(float Max)
{
	MaxHealthCPP = Max;
}

void UDamageSystem::SetHealth(float current)
{
	HealthCPP = current;
}
