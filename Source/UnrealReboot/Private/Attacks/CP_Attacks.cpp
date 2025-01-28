// Fill out your copyright notice in the Description page of Project Settings.


#include "Attacks/CP_Attacks.h"
#include "DrawDebugHelpers.h"//����Ʈ���̽� ������ ��Ŭ��� �Լ�
#include "GameFramework/ProjectileMovementComponent.h"
#include <Kismet/KismetSystemLibrary.h>
#include "GameFramework/Character.h"
#include "Enemies/EnemyAIInterface.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Enemies/EnemyAIInterface.h"
#include "Perception/AIPerceptionComponent.h"
#include "Components/CapsuleComponent.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Components/CapsuleComponent.h"
#include <AOE/AOEBase.h>
#include <Kismet/GameplayStatics.h>

// Sets default values for this component's properties
UCP_Attacks::UCP_Attacks()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCP_Attacks::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UCP_Attacks::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}



TArray<AActor*> UCP_Attacks::DamageAllNonTeamMembers(const TArray<FHitResult>& Hits, FDamageInfo& DamageInfo)
{
    // Hits �迭�� ��� �� ��Ʈ ����� ���� �ݺ�
    for (const FHitResult& Hit : Hits)
    {
        AActor* HitActor = Hit.GetActor();
        if (HitActor && !ActorsDamagedSoFar.Contains(HitActor))
        {
            // HitActor�� DamageableInterface�� �����ߴ��� Ȯ��
            if (HitActor->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()) &&
                GetOwner()->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()))
            {
                // �������̽� �Լ� ȣ��
                int32 ActorTeamNumber = IDamageableInterface::Execute_GetTeamNumber(HitActor);
                int32 OwnerTeamNumber = IDamageableInterface::Execute_GetTeamNumber(GetOwner());

                // ���� �ٸ� ��� Damage ó��
                if (ActorTeamNumber != OwnerTeamNumber)
                {
                    IDamageableInterface::Execute_TakeDamage(HitActor, DamageInfo, GetOwner());
                    ActorsDamagedSoFar.AddUnique(HitActor);
                }
            }
        }
    }

    // �ջ�� ���� ��� ��ȯ
    return ActorsDamagedSoFar;
}

AActor* UCP_Attacks::DamageFirstNonTeamMembers(const TArray<FHitResult>& Hits, FDamageInfo& DamageInfo)
{
    AActor* DamagedActor = nullptr; // ��ȯ�� ���ظ� ���� ���͸� �����ϱ� ���� ����

    for (const FHitResult& Hit : Hits)
    {
        AActor* HitActor = Hit.GetActor();
        if (HitActor && GetOwner())
        {
            // HitActor�� Owner�� DamageableInterface�� �����ߴ��� Ȯ��
            if (HitActor->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()) &&
                GetOwner()->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()))
            {
                // �������̽� �Լ� ȣ��
                int32 ActorTeamNumber = IDamageableInterface::Execute_GetTeamNumber(HitActor);
                int32 OwnerTeamNumber = IDamageableInterface::Execute_GetTeamNumber(GetOwner());

                // �� ��ȣ�� �ٸ� ��� ���� ó��
                if (ActorTeamNumber != OwnerTeamNumber)
                {
                    IDamageableInterface::Execute_TakeDamage(HitActor, DamageInfo, GetOwner());
                    DamagedActor = HitActor; // ���ظ� ���� ���͸� ����
                    break; // ù ��° ���� ������� ���ظ� �־����Ƿ� ������ ����
                }
            }
        }
    }

    return DamagedActor; // ���ظ� ���� ���͸� ��ȯ�ϰų�, ���ٸ� nullptr ��ȯ
}

USkeletalMeshComponent* UCP_Attacks::AttackBase(AActor* AttackTarget)
{
    if (GetOwner()->GetClass()->ImplementsInterface(UEnemyAIInterface::StaticClass()))
    {
        // Attack ȣ��
        IEnemyAIInterface::Execute_Attack(GetOwner(), AttackTarget);
    }

    // Owner�� ACharacter�� ĳ����
    ACharacter* CharOwner = Cast<ACharacter>(GetOwner());
    if (CharOwner)
    {
        return CharOwner->GetMesh();
    }
    else
    {
        return nullptr;
    }
}



FVector UCP_Attacks::CalculateFutureActorLocation(AActor* Actor, float Time)
{
    FVector ForMultifliedVector(1.0, 1.0, 0);
    FVector Location;
    Location = (Actor->GetVelocity()*ForMultifliedVector*Time)+Actor->GetActorLocation();
    return Location;
}

void UCP_Attacks::FireBullet(FVector TraceStart, FVector TraceEnd, FDamageInfo& DamageInfo)
{
    FCollisionQueryParams QueryParams;
    QueryParams.bTraceComplex = true; // If you want to trace against complex collision
    QueryParams.bReturnPhysicalMaterial = false;
    QueryParams.AddIgnoredActor(GetOwner());

    // Prepare the object types array
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

    TArray<FHitResult> OutHits;

    UWorld* World = GetWorld();
    if (!World) return;

    // Perform the multi-line trace
    bool bHit = UKismetSystemLibrary::LineTraceMultiForObjects(
        World,
        TraceStart,
        TraceEnd,
        ObjectTypes,
        false,
        TArray<AActor*>(), 
        EDrawDebugTrace::None,
        OutHits,
        true
    );
    if(bHit&&OutHits.Num()>0)
    {
        AActor* DamagedActor = DamageFirstNonTeamMembers(OutHits, DamageInfo);

        // If DamageFirstNonTeamMembers returns->damagedActor
        if (DamagedActor)
        {
            ReportDamageEvent
            (World, DamagedActor, GetOwner(), DamageInfo.Amount, (GetOwner()->GetActorLocation()), DamagedActor->GetActorLocation());
        }
    }

}

void UCP_Attacks::MagicSpell(FTransform& SpawnTransform, AActor* Target, FDamageInfo& DamageInfo, TSubclassOf<AProjectileBase> ClassToSpawn)
{
    if (!ClassToSpawn)
    {
        UE_LOG(LogTemp, Error, TEXT("ClassToSpawn is null!"));
        return;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("World is null!"));
        return;
    }

    CurrentDamageInfo = DamageInfo;

    // FActorSpawnParameters ����
    FActorSpawnParameters SpawnParams;
    SpawnParams.Name = NAME_None;
    SpawnParams.Owner = GetOwner();
    SpawnParams.Instigator = Cast<APawn>(GetOwner());
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    // �����: SpawnTransform Ȯ��
    UE_LOG(LogTemp, Warning, TEXT("SpawnTransform Location: %s"), *SpawnTransform.GetLocation().ToString());

    // �߻�ü ����
    AProjectileBase* SpawnedProjectile = World->SpawnActor<AProjectileBase>(ClassToSpawn, SpawnTransform, SpawnParams);

    if (SpawnedProjectile)
    {
        // �߻�ü �ʱ�ȭ
        SpawnedProjectile->InitializeProjectile(1000.0f, 0.0f, false, Target);

        // �߻�ü�� �ڽ��� �����ڸ� �����ϵ��� ����
        AActor* OwnerActor = GetOwner();
        ACharacter* OwnerCharacter = Cast<ACharacter>(OwnerActor);
        if (OwnerActor)
        {
            // �߻�ü�� �浹 ������Ʈ���� IgnoreActorWhenMoving ȣ��
            UCapsuleComponent* ProjectileCollisionComponent = OwnerCharacter->GetCapsuleComponent();
            if (ProjectileCollisionComponent)
            {
                ProjectileCollisionComponent->IgnoreActorWhenMoving(OwnerCharacter, true);

                UE_LOG(LogTemp, Warning, TEXT("Projectile is set to ignore its owner: %s"), *OwnerCharacter->GetName());
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Projectile collision component is null!"));
            }
        }

        // �浹 �̺�Ʈ ���ε�
        SpawnedProjectile->OnProjectileImpact.AddDynamic(this, &UCP_Attacks::OnProjectileHit);

        UE_LOG(LogTemp, Warning, TEXT("Projectile spawned: %s"), *SpawnedProjectile->GetName());

    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to spawn projectile! ClassToSpawn: %s"), *ClassToSpawn->GetName());
    }
}

void UCP_Attacks::AOEDamage(float Radius, FDamageInfo& DamageInfo)
{
    //DamageInfoMapping �� �浹���ٴ� ���� ��-> �׷��� ���� �������� ���⶧���� �״�� InfoMapping �״�� ��
    
    UWorld* World = GetWorld();
    if (World)
    {
        
        FVector SpawnLocation = GetOwner()->GetActorLocation();

        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        AAOEBase* SpawnedAOEActor = World->SpawnActor<AAOEBase>(AAOEBase::StaticClass(), SpawnLocation, FRotator::ZeroRotator, SpawnParams);

        if (SpawnedAOEActor)
        {
            SpawnedAOEActor->SetRadius(Radius);
            AOEDamageInfoMapping = FAOEDamageInfoMapping(SpawnedAOEActor, DamageInfo);

            SpawnedAOEActor->OnAOEOverlapActor.AddDynamic(this, &UCP_Attacks::AOEDamageActor);
        
        }
            
    }
}

void UCP_Attacks::AOEDamageActor(AActor* Actor)
{
    if (AOEDamageInfoMapping.IsSet() && AOEDamageInfoMapping.GetValue().AOEDamageActor == Actor)
    {
        FAOEDamageInfoMapping& Mapping = AOEDamageInfoMapping.GetValue();

        // Actor�� Owner�� IDamageableInterface�� �����ߴ��� Ȯ��
        if (Actor->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()) &&
            GetOwner()->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()))
        {
            // �������̽� �Լ� ȣ��
            int32 ActorTeamNumber = IDamageableInterface::Execute_GetTeamNumber(Actor);
            int32 OwnerTeamNumber = IDamageableInterface::Execute_GetTeamNumber(GetOwner());

            // �� ��ȣ�� �ٸ� ��� ���� ó��
            if (ActorTeamNumber != OwnerTeamNumber)
            {
                IDamageableInterface::Execute_TakeDamage(Actor, Mapping.DamageInfo, GetOwner());
                AOEDamageInfoMapping.Reset();
            }
        }
    }
}

void UCP_Attacks::SphereTraceDamage(float Radius, float Length, FDamageInfo& DamageInfo)
{
    UWorld* World = GetWorld();
    if (!World) return;



    FVector StartLocation = GetOwner()->GetActorLocation();
    FVector ForwardVector = GetOwner()->GetActorForwardVector();
    FVector EndLocation = StartLocation + (ForwardVector * Length);

    // �Ķ���͵�
    FCollisionQueryParams TraceParams(FName(TEXT("SphereTraceDamage")), false, GetOwner());
    TraceParams.bReturnPhysicalMaterial = true;

    //Sphere Trace �Ѱ�
    TArray<FHitResult> HitResults;
    bool bHit = World->SweepMultiByChannel(
        HitResults,
        StartLocation,
        EndLocation,
        FQuat::Identity,
        ECC_Pawn,
        FCollisionShape::MakeSphere(Radius),
        TraceParams
    );

    // ������ ���� �Ǻ�
    if (bHit)
    {
        DamageAllNonTeamMembers(HitResults, DamageInfo);
    }
}

void UCP_Attacks::JumpToAttackTarget(AActor* AttackTarget)
{
    if (!AttackTarget || !GetOwner()) return; // ��ȿ�� �˻�

    // 1. ���� ��ġ�� Ÿ�� ��ġ ��������
    FVector StartPosition = GetOwner()->GetActorLocation();
    FVector AttackTargetPosition = AttackTarget->GetActorLocation();

    // 2. Ÿ���� �̷� ��ġ ���
    float PredictionTime = 1.0f; // ���� �ð�
    FVector PredictedTargetLocation = CalculateFutureActorLocation(AttackTarget, PredictionTime);

    // Z ���� 100��ŭ �ø�
    PredictedTargetLocation.Z += 100.0f;

    // 3. �Ÿ� ���
    float Distance = FVector::Dist(StartPosition, AttackTargetPosition);

    // 4. �Ÿ� ������ Normalize (400 ~ 800 ���̷� ����)
    float NormalizedRange = FMath::Clamp((Distance - 400.0f) / 400.0f, 0.0f, 1.0f);

    // 5. �ӵ� ��� (Lerp ���)
    float Alpha = FMath::Lerp(0.5f, 0.94f, NormalizedRange);
    FVector LaunchVelocity;

    // 6. ������ �ӵ� ���
    bool bHasValidVelocity = UGameplayStatics::SuggestProjectileVelocity_CustomArc(
        this,
        LaunchVelocity,
        StartPosition,
        PredictedTargetLocation,
        0.0f // �߷� �������̵� �� (�⺻ �߷� ���)
    );

    if (!bHasValidVelocity)
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to calculate projectile velocity!"));
        return;
    }

    // 7. ���� ����
    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (OwnerCharacter)
    {
        OwnerCharacter->LaunchCharacter(LaunchVelocity, true, true);

        // ���� �̺�Ʈ ���ε�
        OwnerCharacter->LandedDelegate.AddDynamic(this, &UCP_Attacks::OnLand);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Owner is not a character, cannot launch!"));
    }
}

void UCP_Attacks::OnLand(const FHitResult& Hit)
{
    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (OwnerCharacter)
    {
        // ����ε�
        OwnerCharacter->LandedDelegate.RemoveDynamic(this, &UCP_Attacks::OnLand);

        // Stop the character's movement
        OwnerCharacter->GetCharacterMovement()->StopMovementImmediately();
    }
}

void UCP_Attacks::GroundSmash(FAttackInfo& AttackInfo, float Radius)
{
    CurrentAttackInfo = AttackInfo;
    CurrentRadius = Radius;

    USkeletalMeshComponent* MeshComp = AttackBase(AttackInfo.AttackTarget);// �Ž�������Ʈ �޾ƿ���
    UAnimMontage* MontageToPlay = AttackInfo.Montage;

    if (MeshComp && MontageToPlay)
    {
        UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
        if (AnimInstance)
        {
            float MontagePlayRate = 1.0f;
            float MontageStartPosition = 0.0f;
            FName MontageStartingSection= NAME_None; 

            // �÷��� �������Ͻ�
            float MontageLength = AnimInstance->Montage_Play(MontageToPlay, MontagePlayRate, EMontagePlayReturnType::MontageLength, MontageStartPosition);
            bool bPlayedSuccessfully = MontageLength > 0.f;

            if (IDamageableInterface* OwnerDmgInterface = Cast<IDamageableInterface>(GetOwner()))
            {
                IDamageableInterface::Execute_SetIsInterruptible(GetOwner(), false);
            }

            if (bPlayedSuccessfully)
            {
                // Optionally jump to a starting section
                if (MontageStartingSection != NAME_None)
                {
                    AnimInstance->Montage_JumpToSection(MontageStartingSection, MontageToPlay);
                }

                // EndDelegtate ���⼭ ���ε�
                FOnMontageEnded OnMontageEndedDelegate;
                OnMontageEndedDelegate.BindUObject(this, &UCP_Attacks::OnMontageCompleted);



                AnimInstance->Montage_SetEndDelegate(OnMontageEndedDelegate, MontageToPlay);

                // Bind to Notify Begin and End for additional actions
                AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &UCP_Attacks::OnNotifyBeginReceived_GroundSmash);
   
            }
            else
            {
                // Handle montage interrupted or failed to play
                OnInterrupted();
            }
        }
    }
}

void UCP_Attacks::PrimaryMeleeAttack(FAttackInfo& AttackInfo, float Radius, float Length)
{
    CurrentAttackInfo = AttackInfo;
    CurrentRadius = Radius;

    USkeletalMeshComponent* MeshComp = AttackBase(AttackInfo.AttackTarget); // �Ž� ������Ʈ �޾ƿ���
    UAnimMontage* MontageToPlay = AttackInfo.Montage;

    if (MeshComp && MontageToPlay)
    {
        UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
        if (AnimInstance)
        {
            float MontagePlayRate = 1.0f;
            float MontageStartPosition = 0.0f;
            FName MontageStartingSection = NAME_None;

            // �ִϸ��̼� ���
            float MontageLength = AnimInstance->Montage_Play(MontageToPlay, MontagePlayRate, EMontagePlayReturnType::MontageLength, MontageStartPosition);
            bool bPlayedSuccessfully = MontageLength > 0.f;

            // Owner�� DamageableInterface�� �����ߴ��� Ȯ��
            if (GetOwner() && GetOwner()->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()))
            {
                // �������̽� �Լ� ȣ��
                IDamageableInterface::Execute_SetIsInterruptible(GetOwner(), false);
            }

            if (bPlayedSuccessfully)
            {
                // Ư�� �������� �̵�
                if (MontageStartingSection != NAME_None)
                {
                    AnimInstance->Montage_JumpToSection(MontageStartingSection, MontageToPlay);
                }

                // EndDelegate ���ε�
                FOnMontageEnded OnMontageEndedDelegate;
                OnMontageEndedDelegate.BindUObject(this, &UCP_Attacks::OnMontageCompleted);

                AnimInstance->Montage_SetEndDelegate(OnMontageEndedDelegate, MontageToPlay);

                // Notify Begin ���ε�
                AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &UCP_Attacks::OnNotifyBeginReceived_GroundSmash);
            }
            else
            {
                // ��Ÿ�� ��� ���� ó��
                OnInterrupted();
            }
        }
    }
}

void UCP_Attacks::LongRangeMeleeAttack(FAttackInfo& AttackInfo)
{
    CurrentAttackInfo = AttackInfo;

    USkeletalMeshComponent* MeshComp = AttackBase(AttackInfo.AttackTarget); // �Ž� ������Ʈ �޾ƿ���
    UAnimMontage* MontageToPlay = AttackInfo.Montage;

    if (MeshComp && MontageToPlay)
    {
        UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
        if (AnimInstance)
        {
            float MontagePlayRate = 1.0f;
            float MontageStartPosition = 0.0f;
            FName MontageStartingSection = NAME_None; // �ʿ��� ��� ����

            // ��Ÿ�� ��� �� ���� ���� Ȯ��
            float MontageLength = AnimInstance->Montage_Play(MontageToPlay, MontagePlayRate, EMontagePlayReturnType::MontageLength, MontageStartPosition);
            bool bPlayedSuccessfully = MontageLength > 0.f;

            // Owner�� IDamageableInterface�� �����ߴ��� Ȯ��
            if (GetOwner() && GetOwner()->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()))
            {
                // SetIsInterruptible ȣ��
                IDamageableInterface::Execute_SetIsInterruptible(GetOwner(), false);
            }

            if (bPlayedSuccessfully)
            {
                // �ʿ��� ��� Ư�� �������� �̵�
                if (MontageStartingSection != NAME_None)
                {
                    AnimInstance->Montage_JumpToSection(MontageStartingSection, MontageToPlay);
                }

                // OnMontageEnded ��������Ʈ ���ε�
                FOnMontageEnded OnMontageEndedDelegate;
                OnMontageEndedDelegate.BindUObject(this, &UCP_Attacks::OnMontageCompleted);
                AnimInstance->Montage_SetEndDelegate(OnMontageEndedDelegate, MontageToPlay);

                // Notify Begin �̺�Ʈ ���ε�
                AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &UCP_Attacks::OnNotifyBeginReceived_LongRangeMeleeAttack);
            }
            else
            {
                // ��Ÿ�� ��� ���� �Ǵ� �ߴ� ó��
                OnInterrupted();
            }
        }
    }
}

void UCP_Attacks::SpinningMeleeAttack(FAttackInfo& AttackInfo)
{
    CurrentAttackInfo = AttackInfo;

    USkeletalMeshComponent* MeshComp = AttackBase(AttackInfo.AttackTarget); // �Ž� ������Ʈ �޾ƿ���
    UAnimMontage* MontageToPlay = AttackInfo.Montage;

    if (MeshComp && MontageToPlay)
    {
        UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
        if (AnimInstance)
        {
            float MontagePlayRate = 1.0f;
            float MontageStartPosition = 0.0f;
            FName MontageStartingSection = NAME_None; // �ʿ��� ��� ����

            // ��Ÿ�� ��� �� ���� ���� Ȯ��
            float MontageLength = AnimInstance->Montage_Play(MontageToPlay, MontagePlayRate, EMontagePlayReturnType::MontageLength, MontageStartPosition);
            bool bPlayedSuccessfully = MontageLength > 0.f;

            // Owner�� IDamageableInterface�� �����ߴ��� Ȯ��
            if (GetOwner() && GetOwner()->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()))
            {
                // SetIsInterruptible ȣ��
                IDamageableInterface::Execute_SetIsInterruptible(GetOwner(), false);
            }

            if (bPlayedSuccessfully)
            {
                // �ʿ��� ��� Ư�� �������� �̵�
                if (MontageStartingSection != NAME_None)
                {
                    AnimInstance->Montage_JumpToSection(MontageStartingSection, MontageToPlay);
                }

                // OnMontageEnded ��������Ʈ ���ε�
                FOnMontageEnded OnMontageEndedDelegate;
                OnMontageEndedDelegate.BindUObject(this, &UCP_Attacks::OnMontageCompleted);
                AnimInstance->Montage_SetEndDelegate(OnMontageEndedDelegate, MontageToPlay);

                // Notify Begin �̺�Ʈ ���ε�
                AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &UCP_Attacks::OnNotifyBeginReceived_SpinningMeleeAttack);

                // Notify End �̺�Ʈ ���ε�
                FOnMontageEnded OnMontageInterruptedDelegate;
                OnMontageInterruptedDelegate.BindUObject(this, &UCP_Attacks::OnMontageInterrupted);
                AnimInstance->Montage_SetEndDelegate(OnMontageInterruptedDelegate, MontageToPlay);
            }
            else
            {
                // ��Ÿ�� ��� ���� �Ǵ� �ߴ� ó��
                OnInterrupted();
            }
        }
    }
}

void UCP_Attacks::BasicMageSpell(FAttackInfo& AttackInfo)
{
    CurrentAttackInfo = AttackInfo;

    USkeletalMeshComponent* MeshComp = AttackBase(AttackInfo.AttackTarget);// �Ž�������Ʈ �޾ƿ���
    UAnimMontage* MontageToPlay = AttackInfo.Montage;

    if (MeshComp && MontageToPlay)
    {
        UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
        if (AnimInstance)
        {
            float MontagePlayRate = 1.0f;
            float MontageStartPosition = 0.0f;
            FName MontageStartingSection; // Set if you have one

            // Play the montage and check if played successfully
            float MontageLength = AnimInstance->Montage_Play(MontageToPlay, MontagePlayRate, EMontagePlayReturnType::MontageLength, MontageStartPosition);
            bool bPlayedSuccessfully = MontageLength > 0.f;

            if (bPlayedSuccessfully)
            {
                // Optionally jump to a starting section
                if (MontageStartingSection != NAME_None)
                {
                    AnimInstance->Montage_JumpToSection(MontageStartingSection, MontageToPlay);
                }

                // Bind the OnMontageEnded delegate
                FOnMontageEnded OnMontageEndedDelegate;
                OnMontageEndedDelegate.BindUObject(this, &UCP_Attacks::OnMontageCompleted);
                AnimInstance->Montage_SetEndDelegate(OnMontageEndedDelegate, MontageToPlay);

                // Bind to Notify Begin and End for additional actions
                AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &UCP_Attacks::OnNotifyBeginReceived_BasicMageSpell);

            }
            else
            {
                // Handle montage interrupted or failed to play
                OnInterrupted();
            }
        }
    }
}

void UCP_Attacks::BarrageMagicSpell(FAttackInfo& AttackInfo)
{
    CurrentAttackInfo = AttackInfo;

    USkeletalMeshComponent* MeshComp = AttackBase(AttackInfo.AttackTarget);// �Ž�������Ʈ �޾ƿ���
    UAnimMontage* MontageToPlay = AttackInfo.Montage;

    if (MeshComp && MontageToPlay)
    {
        UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
        if (AnimInstance)
        {
            float MontagePlayRate = 1.0f;
            float MontageStartPosition = 0.0f;
            FName MontageStartingSection; // Set if you have one

            // Play the montage and check if played successfully
            float MontageLength = AnimInstance->Montage_Play(MontageToPlay, MontagePlayRate, EMontagePlayReturnType::MontageLength, MontageStartPosition);
            bool bPlayedSuccessfully = MontageLength > 0.f;

            if (bPlayedSuccessfully)
            {
                // Optionally jump to a starting section
                if (MontageStartingSection != NAME_None)
                {
                    AnimInstance->Montage_JumpToSection(MontageStartingSection, MontageToPlay);
                }

                // Bind the OnMontageEnded delegate
                FOnMontageEnded OnMontageEndedDelegate;
                OnMontageEndedDelegate.BindUObject(this, &UCP_Attacks::OnMontageCompleted);
                AnimInstance->Montage_SetEndDelegate(OnMontageEndedDelegate, MontageToPlay);

                // Bind to Notify Begin and End for additional actions
                AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &UCP_Attacks::OnNotifyBeginReceived_BarrageMagicSpell);

            }
            else
            {
                // Handle montage interrupted or failed to play
                OnInterrupted();
            }
        }
    }
}

void UCP_Attacks::OnProjectileHit(AActor* OtherActor, const FHitResult& Hit)
{
    FDamageInfo DamageInfo=CurrentDamageInfo;
    if (OtherActor && OtherActor->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()))
    {
        // TakeDamageȣ��
        IDamageableInterface::Execute_TakeDamage(OtherActor,DamageInfo,GetOwner());
    }
    else
    {
        return;
    }


    UAISense_Damage::ReportDamageEvent(
        this,             // WorldContextObject
        OtherActor,     // ���ݹ��� ����
        GetOwner(),       // ������ ����
        DamageInfo.Amount,     // ��������
        GetOwner()->GetActorLocation(),    // �̺�Ʈ �߻� ��ġ (���� Instigator ��ġ)
        Hit.ImpactPoint,      // �浹 ��ġ
        NAME_None         // �±� (�ʿ��ϸ� ���� ����)
    );
    OnAttackEnd.Broadcast();
}




void UCP_Attacks::ReportDamageEvent(UObject* WorldContextObject, AActor* DamagedActor, AActor* InstigatorActor, float DamageAmount, FVector EventLocation, FVector HitLocation)
{
    if (UAIPerceptionSystem* PerceptionSystem = UAIPerceptionSystem::GetCurrent(WorldContextObject))
    {
        FAIDamageEvent DamageEvent;
        DamageEvent.DamagedActor = DamagedActor;
        DamageEvent.Instigator = InstigatorActor;
        DamageEvent.Amount = DamageAmount;
        DamageEvent.Location = EventLocation;
        DamageEvent.HitLocation = HitLocation;

        PerceptionSystem->OnEvent(DamageEvent);
    }

}

void UCP_Attacks::OnMontageCompleted(UAnimMontage* Montage, bool bInterrupted)
{
    //Interrupted �� ����ٰ� ���� �ִµ�, ������ �ǵ� �ȵǵ� �Ȱ��� OnInterrupted ����Ұ���
    //  �б⹮�� ���ؼ� true�϶� complete �ƴҶ��� interrupted ��
    OnInterrupted();
}

void UCP_Attacks::OnMontageBlendedOut(UAnimMontage* Montage, bool bInterrupted)
{
    if (bInterrupted)
    {
        // Owner�� IDamageableInterface�� �����ߴ��� Ȯ��
        if (GetOwner() && GetOwner()->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()))
        {
            // SetIsInterruptible ȣ��
            IDamageableInterface::Execute_SetIsInterruptible(GetOwner(), true);
        }

        // Owner�� IEnemyAIInterface�� �����ߴ��� Ȯ��
        if (GetOwner() && GetOwner()->GetClass()->ImplementsInterface(UEnemyAIInterface::StaticClass()))
        {
            // AttackEnd ȣ��
            IEnemyAIInterface::Execute_AttackEnd(GetOwner(),CurrentAttackTarget);
        }
    }
    else
    {
        // Blendout ó��
    }
}


void UCP_Attacks::OnNotifyBeginReceived_GroundSmash(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
    if (NotifyName == "Smash")
        AOEDamage(CurrentRadius, CurrentAttackInfo.DamageInfo);
}

void UCP_Attacks::OnNotifyBeginReceived_PrimaryMeleeAttack(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
    if (NotifyName == "Slash")
        SphereTraceDamage(CurrentRadius, CurrentLength, CurrentAttackInfo.DamageInfo);
}

void UCP_Attacks::OnNotifyBeginReceived_LongRangeMeleeAttack(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
    if (NotifyName == "Slash")
        SphereTraceDamage(20.0f, 200.0f, CurrentAttackInfo.DamageInfo);
    if (NotifyName == "Jump")
        JumpToAttackTarget(CurrentAttackInfo.AttackTarget);
}

void UCP_Attacks::OnNotifyBeginReceived_SpinningMeleeAttack(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
    if (NotifyName == "Slash")
        SphereTraceDamage(20.0f,200.0f,CurrentAttackInfo.DamageInfo);
    if (NotifyName == "AOESlash")
        AOEDamage(300.0f,CurrentAttackInfo.DamageInfo);
}
void UCP_Attacks::OnNotifyBeginReceived_BasicMageSpell(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
    if (NotifyName == "Fire")
    {
        FVector TempLocation;
       TempLocation= CurrentMeshComponent->GetSocketLocation("RightHand");
       FTransform TempTransform(FRotator::ZeroRotator, TempLocation, FVector(1.0f, 1.0f, 1.0f));

        MagicSpell(TempTransform,CurrentAttackInfo.AttackTarget,CurrentAttackInfo.DamageInfo,NULL);//�̺κп��� � projectile class �� ���� ��������Ѵ�.
    }
}
void UCP_Attacks::OnNotifyBeginReceived_BarrageMagicSpell(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
    if (NotifyName == "Fire")
    {
        FVector TempLocation;
        TempLocation = CurrentMeshComponent->GetSocketLocation("RightHand");

        AActor* Owner = GetOwner();
        FVector ActorLocation = Owner->GetActorLocation();
        float RandomY = FMath::RandRange(-300.0f, 300.0f);
        FVector NewLocation = FVector(0.0f, RandomY, 0.0f);
        FVector TargetLocation = ActorLocation + NewLocation;
        FRotator LookAtRotation = FRotationMatrix::MakeFromX(TargetLocation - ActorLocation).Rotator();



        FTransform TempTransform(LookAtRotation, TempLocation, FVector(1.0f, 1.0f, 1.0f));
        MagicSpell(TempTransform, CurrentAttackInfo.AttackTarget, CurrentAttackInfo.DamageInfo, false);
    }
}

void UCP_Attacks::OnInterrupted()
{
    if (GetOwner())
    {
        // Owner�� IEnemyAIInterface�� �����ߴ��� Ȯ��
        if (GetOwner()->GetClass()->ImplementsInterface(UEnemyAIInterface::StaticClass()))
        {
            // AttackEnd ȣ��
            IEnemyAIInterface::Execute_AttackEnd(GetOwner(), CurrentAttackTarget);
        }

        // Owner�� IDamageableInterface�� �����ߴ��� Ȯ��
        if (GetOwner()->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()))
        {
            // SetIsInterruptible ȣ��
            IDamageableInterface::Execute_SetIsInterruptible(GetOwner(), true);
        }
    }
}

void UCP_Attacks::OnMontageInterrupted(UAnimMontage* Montage, bool bInterrupted)
{
    if (GetOwner())
    {


        // Owner�� IEnemyAIInterface�� �����ߴ��� Ȯ��
        if (GetOwner()->GetClass()->ImplementsInterface(UEnemyAIInterface::StaticClass()))
        {
            // AttackEnd ȣ��
            IEnemyAIInterface::Execute_AttackEnd(GetOwner(), CurrentAttackTarget);
        }

        // Owner�� IDamageableInterface�� �����ߴ��� Ȯ��
        if (GetOwner()->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()))
        {
            // SetIsInterruptible ȣ��
            IDamageableInterface::Execute_SetIsInterruptible(GetOwner(), true);
        }
    }
}




