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
    // ���� ü�� ��������
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damageable")
    float GetCurrentHealth();

    // ���� ����
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damageable")
    void SetIsInvincible(bool set);

    // �ִ� ü�� ��������
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damageable")
    float GetMaxHealth();

    // ü�� ȸ��
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damageable")
    float Heal(float Amount);

    // �׾����� Ȯ��
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damageable")
    bool IsDead();

    // ���ظ� �Ծ����� ó��
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damageable")
    bool TakeDamage(FDamageInfo& DamageInfo, AActor* DamageCauser);

    // ���� ������ Ȯ��
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damageable")
    bool IsAttacking();

    // ���� ��ū ����
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damageable")
    bool ReserveAttackToken(int Amount);

    // ���� ��ū ��ȯ
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damageable")
    void ReturnAttackToken(int Amount);

    // �� ��ȣ ��������
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damageable")
    int GetTeamNumber();

    // ���ͷ�Ʈ ���� ���� ����
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damageable")
    void SetIsInterruptible(bool Value);
};