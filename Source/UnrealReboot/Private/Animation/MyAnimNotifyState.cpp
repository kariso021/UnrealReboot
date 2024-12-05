// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/MyAnimNotifyState.h"


void UMyAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);
	// Begin ���� ����, ��: �α�, ���� ���� ���� ��
	if (AActor* Owner = MeshComp->GetOwner())
	{
		UE_LOG(LogTemp, Warning, TEXT("Notify State Begin on %s"), *Owner->GetName());
		// ���⿡ ���� ���۽� ������ ������ �߰�
	}
}

void UMyAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);
	// End ���� ����, ��: ���� ���� �ʱ�ȭ
	if (AActor* Owner = MeshComp->GetOwner())
	{
		UE_LOG(LogTemp, Warning, TEXT("Notify State End on %s"), *Owner->GetName());
		// ���⿡ ���� ����� ������ ������ �߰�
	}
}

void UMyAnimNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);
	// Tick ���� ����, ��: �������� ���� üũ�� ������Ʈ
	UE_LOG(LogTemp, Warning, TEXT("Notify State Ticking"));
	// ���⿡ �� ������ ������ ������ �߰�
}