// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile/ProjectileBase.h"
#include "Kismet/KismetMathLibrary.h" //�������� ����ϱ� ���� �������
#include <Kismet/GameplayStatics.h>



// Sets default values
AProjectileBase::AProjectileBase()
{
	// Tick Ȱ��ȭ
	PrimaryActorTick.bCanEverTick = true;

	// �⺻�� �ʱ�ȭ
	Gravity = 0.0f;
	Speed = 1000.0f;
	IsHoming = false;

	// BoxCollision�� �����ϰ� RootComponent�� ����
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	RootComponent = BoxCollision; // RootComponent�� ����

	BoxCollision->SetNotifyRigidBodyCollision(true);
	BoxCollision->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));

	// Arrow ������Ʈ �ʱ�ȭ �� BoxCollision�� ����
	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	Arrow->SetupAttachment(RootComponent); // BoxCollision�� ����

	// Mesh ������Ʈ �ʱ�ȭ �� BoxCollision�� ����
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent); // BoxCollision�� ����

	// ProjectileMovement ������Ʈ �ʱ�ȭ
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->bAutoActivate = true; // �⺻������ Ȱ��ȭ

	// �浹 �̺�Ʈ ���ε�
	BoxCollision->OnComponentHit.AddDynamic(this, &AProjectileBase::OnHit);
}

void AProjectileBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (!ProjectileMovement) return; // ProjectileMovement ��ȿ�� Ȯ��

	ProjectileMovement->InitialSpeed = Speed;
	ProjectileMovement->MaxSpeed = Speed;
	ProjectileMovement->ProjectileGravityScale = Gravity;

	if (IsHoming && Target)
	{
		// Target�� AActor*�� ��� ��Ʈ ������Ʈ�� �����ɴϴ�.
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

	// 1. Ÿ�� �������� ȸ�� (RotateToTarget �Լ��� �ʿ���)
	RotateToTarget();

	// 2. �߻�ü ���� ���� ���
	if (SpawnSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, SpawnSound, GetActorLocation());
	}

	// 3. ������ �Ǵ� Instigator ��������
	AActor* ActorToIgnore = nullptr;

	if (GetInstigator())
	{
		ActorToIgnore = GetInstigator(); // Instigator�� �����ϸ� Instigator ����
	}
	else if (GetOwner())
	{
		ActorToIgnore = GetOwner(); // Instigator�� ������ Owner ����
	}

	// 4. ��ȿ�� ���, �浹 ���� ����
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
		// �ʱ� �ӵ� �� �߷� ����
		ProjectileMovement->InitialSpeed = InputSpeed;
		ProjectileMovement->MaxSpeed = InputSpeed;
		ProjectileMovement->ProjectileGravityScale = InputGravity;

		// Ÿ�� ��ȿ�� üũ
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
			// ���� �̻��� ���� ����
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
