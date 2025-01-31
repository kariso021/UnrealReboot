// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Player_HUD_CPP.h"
#include "../Player/MainPlayer.h"

void UPlayer_HUD_CPP::SetPlayerReference(AMainPlayer* mainplayer)
{
	OwnerPlayer = mainplayer;
}

float UPlayer_HUD_CPP::GetHealthPercent()
{
    if (!OwnerPlayer) return 0.0f; //  널 포인터 체크

    //  인터페이스 구현 여부 확인 (템플릿 방식 사용)
    if (OwnerPlayer->Implements<UDamageableInterface>())
    {
        float CurrentHealth = IDamageableInterface::Execute_GetCurrentHealth(OwnerPlayer);
        float MaxHealth = IDamageableInterface::Execute_GetMaxHealth(OwnerPlayer);

        return (MaxHealth > 0) ? (CurrentHealth / MaxHealth) : 0.0f; // ? 0 나누기 방지
    }

    return 0.0f; //  인터페이스 미구현 시 기본값 반환
}
