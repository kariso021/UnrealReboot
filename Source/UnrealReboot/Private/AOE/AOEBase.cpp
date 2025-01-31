// Fill out your copyright notice in the Description page of Project Settings.


#include "AOE/AOEBase.h"
#include <Kismet/KismetSystemLibrary.h>
#include "DrawDebugHelpers.h"



// Sets default values
AAOEBase::AAOEBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
   radius=300;
   bDrawDebugSphere=false;
   ignoreInstigator=false;
   triggerOnBeginPlay=false;

}

void AAOEBase::SpawnAOESphere()
{
    UWorld* world = GetWorld();
    if (!world)
    {
        return;
    }

    FVector ActorLocation = GetActorLocation();
    TArray<AActor*> OverlappedActors;
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    TArray<AActor*> ActorsToIgnore;

    // 오브젝트 타입 추가
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

    // 오버랩 체크에서 무시할 액터 추가
    ActorsToIgnore.Add(this); // 자기 자신 무시
    if (ignoreInstigator)
    {
        ActorsToIgnore.Add(GetInstigator());
    }

    // 오버랩 체크 실행
    bool bIsOverlap = UKismetSystemLibrary::SphereOverlapActors(
        world,
        ActorLocation,
        radius, // radius는 클래스 내에 정의되어야 함
        ObjectTypes,
        nullptr, // 필요하면 액터 클래스 필터로 대체
        ActorsToIgnore,
        OverlappedActors
    );

    // 오버랩 결과 디버깅
    //UE_LOG(LogTemp, Warning, TEXT("AOE Spawned at: %s | Overlapped Actors Count: %d"), *ActorLocation.ToString(), OverlappedActors.Num());
    if (bDrawDebugSphere)
    {
        // 디버그 구체 그리기
        DrawDebugSphere(
            world,
            ActorLocation,
            radius,
            12, // 분할구간? 세그먼트 수
            FColor::Purple,
            false, // 지속적인 선
            2.0f, // 생명 시간 (좀 더 길게 설정)
            0, // 깊이 우선순위
            2.0f // 두께 (좀 더 두껍게 설정)
        );
    }

        // 오버랩된 액터마다 디버그 메시지 출력 및 디버그 스피어 생성
        for (AActor* OverlappedActor : OverlappedActors)
        {
           /* UE_LOG(LogTemp, Warning, TEXT("AOE Overlapped Actor: %s"), *OverlappedActor->GetName());*/

            // 오버랩된 액터 위치에 작은 디버그 스피어 추가
            if (bDrawDebugSphere)
            {
                DrawDebugSphere(
                    world,
                    OverlappedActor->GetActorLocation(),
                    20.0f, // 작은 구체
                    8,
                    FColor::Yellow,
                    false,
                    2.0f
                );
            }

            // 이벤트 방송 (이벤트가 정상적으로 실행되는지도 확인)
            OnAOEOverlapActor.Broadcast(OverlappedActor);
        }
}

void AAOEBase::Trigger()
{
	SpawnAOESphere();
}

void AAOEBase::SetRadius(float Rad)
{
    radius = Rad;
}

// Called when the game starts or when spawned
void AAOEBase::BeginPlay()
{
    Super::BeginPlay();

    if (triggerOnBeginPlay)
    {
        // 딜레이 타이머 설정
        FTimerHandle TimerHandle;
        float DelayTime = 0.1f; // 2초 딜레이 예제 (필요에 따라 변경 가능)

        GetWorld()->GetTimerManager().SetTimer(
            TimerHandle,
            this,
            &AAOEBase::Trigger,
            DelayTime,
            false // 반복 실행 여부: false = 한 번만 실행
        );
    }



	
}

// Called every frame
void AAOEBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAOEBase::SetOptionOfAOE(bool DrawDebugsphere, bool IgnoreInstigator, bool TriggerOnBeginPlay)
{
    bDrawDebugSphere = DrawDebugsphere; ignoreInstigator = IgnoreInstigator; triggerOnBeginPlay = TriggerOnBeginPlay;
}


