// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/MyAnimNotifyState.h"


void UMyAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);
	// Begin 로직 구현, 예: 로깅, 액터 상태 변경 등
	if (AActor* Owner = MeshComp->GetOwner())
	{
		UE_LOG(LogTemp, Warning, TEXT("Notify State Begin on %s"), *Owner->GetName());
		// 여기에 상태 시작시 실행할 로직을 추가
	}
}

void UMyAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);
	// End 로직 구현, 예: 액터 상태 초기화
	if (AActor* Owner = MeshComp->GetOwner())
	{
		UE_LOG(LogTemp, Warning, TEXT("Notify State End on %s"), *Owner->GetName());
		// 여기에 상태 종료시 실행할 로직을 추가
	}
}

void UMyAnimNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);
	// Tick 로직 구현, 예: 지속적인 상태 체크나 업데이트
	UE_LOG(LogTemp, Warning, TEXT("Notify State Ticking"));
	// 여기에 매 프레임 실행할 로직을 추가
}