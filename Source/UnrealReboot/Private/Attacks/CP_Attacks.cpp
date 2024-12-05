// Fill out your copyright notice in the Description page of Project Settings.


#include "Attacks/CP_Attacks.h"
#include "DrawDebugHelpers.h"//����Ʈ���̽� ������ ��Ŭ��� �Լ�
#include "Components/PrimitiveComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include <Kismet/KismetSystemLibrary.h>
#include "GameFramework/Character.h"
#include "Enemies/EnemyAIInterface.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Enemies/EnemyAIInterface.h"
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
            IDamageableInterface* DamageableActor = Cast<IDamageableInterface>(HitActor);//�̹� �ص׾���...
            IDamageableInterface* DamageableOwner = Cast<IDamageableInterface>(GetOwner());

            if (DamageableActor && DamageableOwner && DamageableActor->GetTeamNumber() != DamageableOwner->GetTeamNumber())
            {
                DamageableActor->TakeDamage(DamageInfo, GetOwner());

                ActorsDamagedSoFar.AddUnique(HitActor);
            }
        }
    }
    return ActorsDamagedSoFar;
}

AActor* UCP_Attacks::DamageFirstNonTeamMembers(const TArray<FHitResult>& Hits, FDamageInfo& DamageInfo)
{
    AActor* DamagedActor = nullptr; // ��ȯ�� ���ظ� ���� ���͸� �����ϱ� ���� ����

    for (const FHitResult& Hit : Hits)
    {
        AActor* HitActor = Hit.GetActor();
        IDamageableInterface* DamageableActor = Cast<IDamageableInterface>(HitActor);
        IDamageableInterface* DamageableOwner = Cast<IDamageableInterface>(GetOwner());

        if (DamageableActor && DamageableOwner && DamageableActor->GetTeamNumber() != DamageableOwner->GetTeamNumber())
        {
            DamageableActor->TakeDamage(DamageInfo, GetOwner());
            DamagedActor = HitActor; // ���ظ� ���� ���͸� ����
            break; // ù ��° ���� ������� ���ظ� �־����Ƿ� ������ ����
        }
    }

    return DamagedActor; // ���ظ� ���� ���͸� ��ȯ�ϰų�, ���� ���ٸ� nullptr�� ��ȯ
}

USkeletalMeshComponent* UCP_Attacks::AttackBase(AActor* AttackTarget)
{
    IEnemyAIInterface* Owner = Cast<IEnemyAIInterface>(GetOwner());
    Owner->Attack(AttackTarget);
    ACharacter* CharOwner = Cast<ACharacter>(GetOwner());
    if (CharOwner)
        return CharOwner->GetMesh();
    else
        return nullptr;
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

void UCP_Attacks::MagicSpell(FTransform& SpawnTransform, AActor* Target, FDamageInfo& DamageInfo, bool Alt)
{
    UWorld* World = GetWorld();
    if (World != nullptr)
    {
        AProjectileBase* Projectile = World->SpawnActor<AProjectileBase>(
            AProjectileBase::StaticClass(),
            SpawnTransform.GetLocation(),
            SpawnTransform.GetRotation().Rotator(),
            FActorSpawnParameters());

        if (Projectile != nullptr)
        {
            // ���� �߰�
            DamageInfoMappings.Add(FDamageInfoMapping(Projectile, DamageInfo));

            // ������Ÿ���� �ӵ� ����

            // �̺�Ʈ �ڵ鷯 ����
            Projectile->OnProjectileImpact.AddDynamic(this, &UCP_Attacks::OnProjectileHit);

            // �������Ʈ���� ������ 'Ignore Actor when Moving' ��忡 �ش��ϴ� �κ�
            UPrimitiveComponent* ProjectileComponent = Projectile->FindComponentByClass<UPrimitiveComponent>();
            if (ProjectileComponent != nullptr)
            {
                // �����ڸ� �����ϵ��� ����
                AActor* OwnerActor = GetOwner();
                if (OwnerActor)
                {
                    ProjectileComponent->MoveIgnoreActors.Add(OwnerActor);
                }
            }
        }
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
        IDamageableInterface* DamageActor = Cast<IDamageableInterface>(Actor);
        IDamageableInterface* Owner = Cast<IDamageableInterface>(GetOwner());

        if (DamageActor && Owner && DamageActor->GetTeamNumber() != Owner->GetTeamNumber())
        {
            DamageActor->TakeDamage(Mapping.DamageInfo, GetOwner());
            AOEDamageInfoMapping.Reset();
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
    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (OwnerCharacter && AttackTarget)
    {
        FVector StartPos = OwnerCharacter->GetActorLocation();
        FVector EndPos = CalculateFutureActorLocation(AttackTarget, 1.0f); // 1�� ����

        FVector LaunchVelocity;
        if (UGameplayStatics::SuggestProjectileVelocity_CustomArc(
            this,
            LaunchVelocity,
            StartPos,
            EndPos,
            100.0f))
        {
            OwnerCharacter->LaunchCharacter(LaunchVelocity, true, true);

            // Land �Ҷ� ���ε�
            OwnerCharacter->LandedDelegate.AddDynamic(this,&UCP_Attacks::OnLand);
        }
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

            IDamageableInterface* owner_dmg_interface = Cast<IDamageableInterface>(GetOwner());
            owner_dmg_interface->SetIsInterruptible(false);

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
    CurrentLength = Length;
    

    USkeletalMeshComponent* MeshComp = AttackBase(AttackInfo.AttackTarget);// �Ž�������Ʈ �޾ƿ���
    UAnimMontage* MontageToPlay = AttackInfo.Montage;

    if (MeshComp && MontageToPlay)
    {
        UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
        if (AnimInstance)
        {
            float MontagePlayRate = 1.0f;
            float MontageStartPosition = 0.0f;
            FName MontageStartingSection;

            // Play the montage and check if played successfully
            float MontageLength = AnimInstance->Montage_Play(MontageToPlay, MontagePlayRate, EMontagePlayReturnType::MontageLength, MontageStartPosition);
            bool bPlayedSuccessfully = MontageLength > 0.f;

            IDamageableInterface* owner_dmg_interface = Cast<IDamageableInterface>(GetOwner());
            owner_dmg_interface->SetIsInterruptible(false);

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
                AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &UCP_Attacks::OnNotifyBeginReceived_PrimaryMeleeAttack);

            }
            else
            {
                // Handle montage interrupted or failed to play
                OnInterrupted();
            }
        }

        IDamageableInterface* owner_dmg_interface = Cast<IDamageableInterface>(GetOwner());
        owner_dmg_interface->SetIsInterruptible(false);
    }
}

void UCP_Attacks::LongRangeMeleeAttack(FAttackInfo& AttackInfo)
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

            IDamageableInterface* owner_dmg_interface = Cast<IDamageableInterface>(GetOwner());
            owner_dmg_interface->SetIsInterruptible(false);

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
                AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &UCP_Attacks::OnNotifyBeginReceived_LongRangeMeleeAttack);

            }
            else
            {
                // Handle montage interrupted or failed to play
                OnInterrupted();
            }
        }
    }
}

void UCP_Attacks::SpinningMeleeAttack(FAttackInfo& AttackInfo)
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

            IDamageableInterface* owner_dmg_interface = Cast<IDamageableInterface>(GetOwner());
            owner_dmg_interface->SetIsInterruptible(false);

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
                AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &UCP_Attacks::OnNotifyBeginReceived_SpinningMeleeAttack);

            }
            else
            {
                // Handle montage interrupted or failed to play
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
    for (int32 Index = 0; Index < DamageInfoMappings.Num(); ++Index)
    {
        if (DamageInfoMappings[Index].Projectile == OtherActor)
        {
            // ������ ���� ���
            FDamageInfo& DamageInfo = DamageInfoMappings[Index].DamageInfo;
            // ������ ó�� ����...

            // ��� �� ���� ����
            DamageInfoMappings.RemoveAt(Index);
            break;
        }
    }

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
        IDamageableInterface* owner_dmg_interface = Cast<IDamageableInterface>(GetOwner());
        IEnemyAIInterface* owner_AI_interface = Cast<IEnemyAIInterface>(GetOwner());

        /* owner_AI_interface->AttackEnd();*/
        owner_dmg_interface->SetIsInterruptible(true);

    }
    else
    {
        //Blendout
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

        MagicSpell(TempTransform,CurrentAttackInfo.AttackTarget,CurrentAttackInfo.DamageInfo,false);
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
    IEnemyAIInterface* owner_AI_interface = Cast<IEnemyAIInterface>(GetOwner());
    IDamageableInterface* owner_dmg_interface = Cast<IDamageableInterface>(GetOwner());

    owner_AI_interface->AttackEnd(CurrentAttackTarget);
    owner_dmg_interface->SetIsInterruptible(true);
}




