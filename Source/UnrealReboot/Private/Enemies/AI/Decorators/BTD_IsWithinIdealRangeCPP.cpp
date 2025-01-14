// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/AI/Decorators/BTD_IsWithinIdealRangeCPP.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Actor.h"


bool UBTD_IsWithinIdealRangeCPP::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    // ������ ������Ʈ ��������
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComp)
    {
        return false;
    }

    // ��ǥ ��������
    AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetKey.SelectedKeyName));
    if (!TargetActor)
    {
        return false;
    }

    // �̻����� �Ÿ� ��������
    float IdealRange = BlackboardComp->GetValueAsFloat(IdealRangeKey.SelectedKeyName);
    float DistanceToTarget = OwnerComp.GetAIOwner()->GetPawn()->GetDistanceTo(TargetActor);

    // �Ÿ��� ���� ���� ��
    return FMath::Abs(DistanceToTarget - IdealRange) <= ErrorMargin;
}
