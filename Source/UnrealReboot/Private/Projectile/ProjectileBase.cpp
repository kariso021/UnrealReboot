// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile/ProjectileBase.h"
#include "Kismet/KismetMathLibrary.h" //단위벡터 계산하기 위한 헤더파일
#include <Kismet/GameplayStatics.h>



// Sets default values
AProjectileBase::AProjectileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Gravity = 0.0f;
	Speed = 500.0f;
	IsHoming = false;
	
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));

	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	Arrow->SetupAttachment(BoxCollision);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(BoxCollision);

	// ProjectileMovement 컴포넌트 생성 및 초기화
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));


	BoxCollision->SetNotifyRigidBodyCollision(true);
	BoxCollision->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));


	//이벤트 바인딩
	BoxCollision->OnComponentHit.AddDynamic(this, &AProjectileBase::OnHit);
}

// Called when the game starts or when spawned
void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	

}

void AProjectileBase::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	OnProjectileImpact.Broadcast(OtherActor,Hit);
}

void AProjectileBase::RotateTotarget()
{
	if (IsValid(Target)) // 대상이 유효한지 확인
	{
		FVector SelfLocation = GetActorLocation(); // 자신의 위치
		FVector TargetLocation = Target->GetActorLocation(); // 대상의 위치
		FVector Direction = UKismetMathLibrary::GetDirectionUnitVector(SelfLocation, TargetLocation); // 두 위치 사이의 단위 방향 벡터를 계산

		// 발사체 움직임 컴포넌트의 속도를 설정
		if (ProjectileMovement)
		{
			ProjectileMovement->Velocity = Direction * Speed;
		}
	}
}

void AProjectileBase::SpawnImpactEffect(FVector Location)
{
	if (ImpactEffect) // Check if the ImpactEffect is valid
	{
		// Spawn the particle effect at the given location
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ImpactEffect,
			Location,
			FRotator::ZeroRotator, // No rotation
			FVector(1.0f, 1.0f, 1.0f), // Default scale
			true, // bAutoDestroy
			EPSCPoolMethod::None, // Pooling method
			true // bAutoActivate
		);
	}
}

void AProjectileBase::PlayImpactSound(FVector Location)
{
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, Location);
	}
}

void AProjectileBase::PlaySpawnSound()
{
	if (SpawnSound) 
	{
		FVector ActorLocation = GetActorLocation();
		UGameplayStatics::PlaySoundAtLocation(this, SpawnSound, ActorLocation);
	}
}

// Called every frame
void AProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AProjectileBase::InitHoming(USceneComponent* TargetComponent)
{
	HomingTargetComponent = TargetComponent;
	IsHoming = true;
}

void AProjectileBase::UpdateHoming(float DeltaTime)
{
	if (HomingTargetComponent)
	{
		FVector WantDirection = HomingTargetComponent->GetComponentLocation() - GetActorLocation();
		WantDirection.Normalize();

		FVector WantVelocity = WantDirection * ProjectileMovement->MaxSpeed;//속도
		FVector ControllDirection = (WantVelocity - CurrentVelocity).GetClampedToMaxSize(HomingAcceleration * DeltaTime);

		CurrentVelocity += ControllDirection;
		CurrentVelocity = CurrentVelocity.GetClampedToMaxSize(ProjectileMovement->MaxSpeed);//GetClampToMaxsize->최대속도제한->가속이 일정량 넘어가지 않게

		FVector NewLocation = GetActorLocation() + CurrentVelocity * DeltaTime;
		SetActorLocation(NewLocation);
	}
}
