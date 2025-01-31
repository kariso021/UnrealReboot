// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Player_HUD_CPP.h"
#include "../Player/MainPlayer.h"

void UPlayer_HUD_CPP::SetPlayerReference(AMainPlayer* mainplayer)
{
	OwnerPlayer = mainplayer;
}

float UPlayer_HUD_CPP::GetHealthPercent()
{
    if (!OwnerPlayer) return 0.0f; //  �� ������ üũ

    //  �������̽� ���� ���� Ȯ�� (���ø� ��� ���)
    if (OwnerPlayer->Implements<UDamageableInterface>())
    {
        float CurrentHealth = IDamageableInterface::Execute_GetCurrentHealth(OwnerPlayer);
        float MaxHealth = IDamageableInterface::Execute_GetMaxHealth(OwnerPlayer);

        return (MaxHealth > 0) ? (CurrentHealth / MaxHealth) : 0.0f; // ? 0 ������ ����
    }

    return 0.0f; //  �������̽� �̱��� �� �⺻�� ��ȯ
}
