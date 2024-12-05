// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageSystem/DamageableInterface.h"

// Add default functionality here for any IDamageableInterface functions that are not pure virtual.

float IDamageableInterface::GetCurrentHealth()
{
	return 0.0f;
}

void IDamageableInterface::SetIsInvincible(bool set)
{
}

float IDamageableInterface::GetMaxHealth()
{
	return 0.0f;
}

float IDamageableInterface::Heal(float Amount)
{
	return 0.0f;
}

bool IDamageableInterface::IsDead()
{
	return false;
}

bool IDamageableInterface::TakeDamage(FDamageInfo& DamageInfo, AActor* DamageCauser)
{
	return false;
}

bool IDamageableInterface::IsAttacking()
{
	return false;
}

bool IDamageableInterface::ReserveAttackToken(int Amount)
{
	return false;
}

void IDamageableInterface::ReturnAttackToken(int Amount)
{
}

int IDamageableInterface::GetTeamNumber()
{
	return 0;
}

void IDamageableInterface::SetIsInterruptible(bool Value)
{
}
