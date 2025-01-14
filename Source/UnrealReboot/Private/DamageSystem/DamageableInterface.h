// DamageableInterface.h

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

public:
    // 현재 체력 가져오기
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damageable")
    float GetCurrentHealth();

    // 무적 설정
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damageable")
    void SetIsInvincible(bool set);

    // 최대 체력 가져오기
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damageable")
    float GetMaxHealth();

    // 체력 회복
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damageable")
    float Heal(float Amount);

    // 죽었는지 확인
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damageable")
    bool IsDead();

    // 피해를 입었는지 처리
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damageable")
    bool TakeDamage(FDamageInfo& DamageInfo, AActor* DamageCauser);

    // 공격 중인지 확인
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damageable")
    bool IsAttacking();

    // 공격 토큰 예약
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damageable")
    bool ReserveAttackToken(int Amount);

    // 공격 토큰 반환
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damageable")
    void ReturnAttackToken(int Amount);

    // 팀 번호 가져오기
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damageable")
    int GetTeamNumber();

    // 인터럽트 가능 여부 설정
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damageable")
    void SetIsInterruptible(bool Value);
};