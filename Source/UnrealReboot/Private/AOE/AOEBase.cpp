// Fill out your copyright notice in the Description page of Project Settings.


#include "AOE/AOEBase.h"
#include <Kismet/KismetSystemLibrary.h>



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

    //������Ʈ Ÿ�� �߰�
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

    // ������ üũ���� ������ ���� �߰�
    ActorsToIgnore.Add(this); // �ڱ� �ڽ� ����

    // ������ üũ ����
    bool bIsOverlap = UKismetSystemLibrary::SphereOverlapActors(
        world,
        ActorLocation,
        radius, // radius�� Ŭ���� ���� ���ǵǾ�� ��
        ObjectTypes,
        nullptr, // �ʿ��ϸ� ���� Ŭ���� ���ͷ� ��ü
        ActorsToIgnore,
        OverlappedActors
    );

    if (bDrawDebugSphere)
    {
        // ����� ��ü �׸���
        DrawDebugSphere(
            world,
            ActorLocation,
            radius,
            12, // ���ұ���? ���׸�Ʈ ��
            FColor::Purple,
            false, // �������� ��
            1.0f, // ���� �ð�
            0, // ���� �켱����
            1.0f // �β�
        );

        // �������� ���͸��� �̺�Ʈ ���
        for (AActor* OverlappedActor : OverlappedActors)
        {
            OnAOEOverlapActor.Broadcast(OverlappedActor);
        }
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
        // ������ Ÿ�̸� ����
        FTimerHandle TimerHandle;
        float DelayTime = 0.1f; // 2�� ������ ���� (�ʿ信 ���� ���� ����)

        GetWorld()->GetTimerManager().SetTimer(
            TimerHandle,
            this,
            &AAOEBase::Trigger,
            DelayTime,
            false // �ݺ� ���� ����: false = �� ���� ����
        );
    }



	
}

// Called every frame
void AAOEBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
