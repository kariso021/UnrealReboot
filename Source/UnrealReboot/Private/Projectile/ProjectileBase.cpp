// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile/ProjectileBase.h"
#include "Kismet/KismetMathLibrary.h" //�������� ����ϱ� ���� �������
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

	// ProjectileMovement ������Ʈ ���� �� �ʱ�ȭ
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));


	BoxCollision->SetNotifyRigidBodyCollision(true);
	BoxCollision->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));


	//�̺�Ʈ ���ε�
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
	if (IsValid(Target)) // ����� ��ȿ���� Ȯ��
	{
		FVector SelfLocation = GetActorLocation(); // �ڽ��� ��ġ
		FVector TargetLocation = Target->GetActorLocation(); // ����� ��ġ
		FVector Direction = UKismetMathLibrary::GetDirectionUnitVector(SelfLocation, TargetLocation); // �� ��ġ ������ ���� ���� ���͸� ���

		// �߻�ü ������ ������Ʈ�� �ӵ��� ����
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

		FVector WantVelocity = WantDirection * ProjectileMovement->MaxSpeed;//�ӵ�
		FVector ControllDirection = (WantVelocity - CurrentVelocity).GetClampedToMaxSize(HomingAcceleration * DeltaTime);

		CurrentVelocity += ControllDirection;
		CurrentVelocity = CurrentVelocity.GetClampedToMaxSize(ProjectileMovement->MaxSpeed);//GetClampToMaxsize->�ִ�ӵ�����->������ ������ �Ѿ�� �ʰ�

		FVector NewLocation = GetActorLocation() + CurrentVelocity * DeltaTime;
		SetActorLocation(NewLocation);
	}
}
