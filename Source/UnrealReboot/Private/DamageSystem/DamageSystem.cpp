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
	if (CanBeDamaged(DamageInfo.ShouldDamageInvincible, DamageInfo.CanBeBlocked) == EDamageResult::Block)
	{
		//Block Damage
		OnBlocked.Broadcast(DamageInfo.CanBeParried, DamageCauser);
		return false;

	}
	else if (CanBeDamaged(DamageInfo.ShouldDamageInvincible, DamageInfo.CanBeBlocked) == EDamageResult::Damage)
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
	else if (CanBeDamaged(DamageInfo.ShouldDamageInvincible, DamageInfo.CanBeBlocked) == EDamageResult::NoDamage)
	{
		//No Damage
		return false;
	}
	return false; //CanBe Damaged �Լ��� ������ ������ False ��ȯ EDasmageResult�� �� ����� ��ȯ���� ��������
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

EDamageResult UDamageSystem::CanBeDamaged(bool IsInvincible, bool CanBeBlocked)
{
	if (!IsDeadCPP && (!IsInvincibleCPP || IsInvincible))
	{
		if (IsBlockingCPP && CanBeBlocked)
		{
			return EDamageResult::Block; // Blocked
		}
		else
		{
			return EDamageResult::Damage; // Do Damage
		}
	}
	else
	{
		return EDamageResult::NoDamage; // No Damage
	}
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


