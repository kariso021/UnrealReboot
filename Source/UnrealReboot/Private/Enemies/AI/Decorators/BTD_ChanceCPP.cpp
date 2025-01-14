// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/AI/Decorators/BTD_ChanceCPP.h"

bool UBTD_ChanceCPP::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
        // 0.0 ~ 1.0 사이의 랜덤 값을 생성
        float RandomValue = FMath::FRand();

        // 랜덤 값이 Chance보다 크거나 같은지 확인
        return RandomValue <= Percent;
}
