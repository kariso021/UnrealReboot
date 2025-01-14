// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/AI/Decorators/BTD_ChanceCPP.h"

bool UBTD_ChanceCPP::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
        // 0.0 ~ 1.0 ������ ���� ���� ����
        float RandomValue = FMath::FRand();

        // ���� ���� Chance���� ũ�ų� ������ Ȯ��
        return RandomValue <= Percent;
}
