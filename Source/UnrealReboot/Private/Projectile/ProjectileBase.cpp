// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile/ProjectileBase.h"
#include "Kismet/KismetMathLibrary.h" //단위벡터 계산하기 위한 헤더파일
#include <Kismet/GameplayStatics.h>



// Sets default values
AProjectileBase::AProjectileBase()
{
	// Tick 활성화
	PrimaryActorTick.bCanEverTick = true;

	// 기본값 초기화
	Gravity = 0.0f;
	Speed = 1000.0f;
	IsHoming = false;

	// BoxCollision을 생성하고 RootComponent로 설정
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	RootComponent = BoxCollision; // RootComponent로 설정

	BoxCollision->SetNotifyRigidBodyCollision(true);
	BoxCollision->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));

	// Arrow 컴포넌트 초기화 및 BoxCollision에 연결
	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	Arrow->SetupAttachment(RootComponent); // BoxCollision에 연결

	// Mesh 컴포넌트 초기화 및 BoxCollision에 연결
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent); // BoxCollision에 연결

	// ProjectileMovement 컴포넌트 초기화
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->bAutoActivate = true; // 기본적으로 활성화

	// 충돌 이벤트 바인딩
	BoxCollision->OnComponentHit.AddDynamic(this, &AProjectileBase::OnHit);
}

void AProjectileBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (!ProjectileMovement) return; // ProjectileMovement 유효성 확인

	ProjectileMovement->InitialSpeed = Speed;
	ProjectileMovement->MaxSpeed = Speed;
	ProjectileMovement->ProjectileGravityScale = Gravity;

	if (IsHoming && Target)
	{
		// Target이 AActor*일 경우 루트 컴포넌트를 가져옵니다.
		USceneComponent* TargetComponent = Target->GetRootComponent();
		if (TargetComponent)
		{
			ProjectileMovement->HomingTargetComponent = TargetComponent;
			ProjectileMovement->bIsHomingProjectile = true;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Target does not have a valid RootComponent."));
		}
	}



}

// Called when the game starts or when spawned
void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();

	// 1. 타겟 방향으로 회전 (RotateToTarget 함수가 필요함)
	RotateToTarget();

	// 2. 발사체 스폰 사운드 재생
	if (SpawnSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, SpawnSound, GetActorLocation());
	}

	// 3. 소유자 또는 Instigator 가져오기
	AActor* ActorToIgnore = nullptr;

	if (GetInstigator())
	{
		ActorToIgnore = GetInstigator(); // Instigator가 존재하면 Instigator 선택
	}
	else if (GetOwner())
	{
		ActorToIgnore = GetOwner(); // Instigator가 없으면 Owner 선택
	}

	// 4. 유효한 경우, 충돌 무시 설정
	if (ActorToIgnore && BoxCollision)
	{
		BoxCollision->IgnoreActorWhenMoving(ActorToIgnore, true);
		UE_LOG(LogTemp, Warning, TEXT("Projectile ignoring collision with: %s"), *ActorToIgnore->GetName());
	}
	

}

void AProjectileBase::InitializeProjectile(float InputSpeed, float InputGravity, bool InputIsHoming, AActor* InputTarget)
{
	if (ProjectileMovement)
	{
		// 초기 속도 및 중력 설정
		ProjectileMovement->InitialSpeed = InputSpeed;
		ProjectileMovement->MaxSpeed = InputSpeed;
		ProjectileMovement->ProjectileGravityScale = InputGravity;

		// 타겟 유효성 체크
		if (InputTarget && InputTarget->IsValidLowLevel())
		{
			USceneComponent* TargetComponent = InputTarget->GetRootComponent();
			if (TargetComponent)
			{
				ProjectileMovement->HomingTargetComponent = TargetComponent;
				ProjectileMovement->bIsHomingProjectile = InputIsHoming;

				UE_LOG(LogTemp, Warning, TEXT("Homing projectile initialized: Speed = %f, Gravity = %f, HomingTarget = %s"),
					InputSpeed, InputGravity, *TargetComponent->GetName());
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Homing Target Component is null!"));
			}
		}
		else
		{
			// 유도 미사일 설정 해제
			ProjectileMovement->HomingTargetComponent = nullptr;
			ProjectileMovement->bIsHomingProjectile = false;

			UE_LOG(LogTemp, Warning, TEXT("No valid target provided for homing projectile."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ProjectileMovement is null!"));
	}
}

void AProjectileBase::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Display, TEXT("Onhit!"));
	UE_LOG(LogTemp, Warning, TEXT("Projectile hit: %s (Component: %s)"),
		*OtherActor->GetName(), *OtherComp->GetName());
	OnProjectileImpact.Broadcast(OtherActor,Hit);
	
	SpawnImpactEffect(Hit.Location);
	PlayImpactSound(Hit.Location);
	Destroy();

}

void AProjectileBase::RotateToTarget()
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
