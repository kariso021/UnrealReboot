// Fill out your copyright notice in the Description page of Project Settings.


#include "Attacks/CP_Attacks.h"
#include "DrawDebugHelpers.h"//라인트레이스 디버깅용 인클루드 함수
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
#include "DrawDebugHelpers.h"
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

    if (ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner()))
    {
        if (UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance())
        {
            // 컴포넌트 시작 시, 모든 노티파이를 수신할 단일 핸들러를 '한 번만' 등록합니다.
            AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &UCP_Attacks::HandleMontageNotify);
        }
    }
	
}

void UCP_Attacks::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // 컴포넌트 종료 시, 등록했던 핸들러를 안전하게 해제합니다.
    if (ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner()))
    {
        if (UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance())
        {
            AnimInstance->OnPlayMontageNotifyBegin.RemoveDynamic(this, &UCP_Attacks::HandleMontageNotify);
        }
    }

    Super::EndPlay(EndPlayReason);
}


// Called every frame
void UCP_Attacks::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}



TArray<AActor*> UCP_Attacks::DamageAllNonTeamMembers(const TArray<FHitResult>& Hits, const FDamageInfo& DamageInfo)
{
    TArray<AActor*> ActorsDamagedSoFar;
    
	FDamageInfo CachedDamageInfo = DamageInfo; // DamageInfo를 캐시하여 사용

    // Hits 배열에 담긴 각 히트 결과에 대해 반복
    for (const FHitResult& Hit : Hits)
    {
        AActor* HitActor = Hit.GetActor();
        if (HitActor && !ActorsDamagedSoFar.Contains(HitActor))
        {
            // HitActor가 DamageableInterface를 구현했는지 확인
            if (HitActor->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()) &&
                GetOwner()->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()))
            {
                // 인터페이스 함수 호출
                int32 ActorTeamNumber = IDamageableInterface::Execute_GetTeamNumber(HitActor);
                int32 OwnerTeamNumber = IDamageableInterface::Execute_GetTeamNumber(GetOwner());

                // 팀이 다를 경우 Damage 처리
                if (ActorTeamNumber != OwnerTeamNumber)
                {
                    IDamageableInterface::Execute_TakeDamage(HitActor, CachedDamageInfo, GetOwner());
                    ActorsDamagedSoFar.AddUnique(HitActor);
                }
            }
        }
    }
    // 데미지 입은 액터 목록 반환
    return ActorsDamagedSoFar;
}

AActor* UCP_Attacks::DamageFirstNonTeamMembers(const TArray<FHitResult>& Hits, FDamageInfo& DamageInfo)
{
    AActor* DamagedActor = nullptr; // 반환할 피해를 입은 액터를 저장하기 위한 변수

    for (const FHitResult& Hit : Hits)
    {
        AActor* HitActor = Hit.GetActor();
        if (HitActor && GetOwner())
        {
            // HitActor와 Owner가 DamageableInterface를 구현했는지 확인
            if (HitActor->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()) &&
                GetOwner()->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()))
            {
                // 인터페이스 함수 호출
                int32 ActorTeamNumber = IDamageableInterface::Execute_GetTeamNumber(HitActor);
                int32 OwnerTeamNumber = IDamageableInterface::Execute_GetTeamNumber(GetOwner());

                // 팀 번호가 다를 경우 피해 처리
                if (ActorTeamNumber != OwnerTeamNumber)
                {
                    IDamageableInterface::Execute_TakeDamage(HitActor, DamageInfo, GetOwner());
                    DamagedActor = HitActor; // 피해를 입은 액터를 저장
                    break; // 첫 번째 비팀 멤버에게 피해를 주었으므로 루프를 종료
                }
            }
        }
    }

    return DamagedActor; // 피해를 입은 액터를 반환하거나, 없다면 nullptr 반환
}

USkeletalMeshComponent* UCP_Attacks::AttackBase(AActor* AttackTarget)
{
    if (GetOwner()->GetClass()->ImplementsInterface(UEnemyAIInterface::StaticClass()))
    {
        // Attack 호출
        IEnemyAIInterface::Execute_Attack(GetOwner(), AttackTarget);
    }

    // Owner를 ACharacter로 캐스팅
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

void UCP_Attacks::MagicSpell(FTransform& SpawnTransform, AActor* Target, FDamageInfo& DamageInfo)
{
    if (!ProjectileSpawnClass)
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

    // FActorSpawnParameters 설정
    FActorSpawnParameters SpawnParams;
    SpawnParams.Name = NAME_None;
    SpawnParams.Owner = GetOwner();
    SpawnParams.Instigator = Cast<APawn>(GetOwner());
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    // 발사체 스폰
    AProjectileBase* SpawnedProjectile = World->SpawnActor<AProjectileBase>(ProjectileSpawnClass, SpawnTransform, SpawnParams);

    if (SpawnedProjectile)
    {
        // 발사체 초기화
        SpawnedProjectile->InitializeProjectile(1000.0f, 0.0f, false, Target);

        // 발사체가 자신의 소유자를 무시하도록 설정
        AActor* OwnerActor = GetOwner();
        ACharacter* OwnerCharacter = Cast<ACharacter>(OwnerActor);
        if (OwnerActor)
        {
            // 발사체의 충돌 컴포넌트에서 IgnoreActorWhenMoving 호출
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

        //충돌 이벤트 and 기존 바인딩 제거
        SpawnedProjectile->OnProjectileImpact.RemoveDynamic(this, &UCP_Attacks::OnProjectileHit);
        SpawnedProjectile->OnProjectileImpact.AddDynamic(this, &UCP_Attacks::OnProjectileHit);

    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to spawn projectile! ClassToSpawn: %s"), *ProjectileSpawnClass->GetName());
    }
}

void UCP_Attacks::AOEDamage(float Radius, const FDamageInfo& DamageInfo)
{
    UWorld* World = GetWorld();
    if (!World || !AOEClass) return;

    // 기존 DamageInfo를 캐싱하여 저장
    AOECachedDamageInfo = DamageInfo;

    FVector SpawnLocation = GetOwner()->GetActorLocation();
    FRotator SpawnRotation = FRotator::ZeroRotator;

    AAOEBase* AOEInstance = World->SpawnActor<AAOEBase>(AOEClass, SpawnLocation, SpawnRotation);
    if (AOEInstance)
    {
        AOEInstance->SetRadius(Radius);
        AOEInstance->SetInstigator(Cast<APawn>(GetOwner()));
        AOEInstance->SetOptionOfAOE(false, true, false);

        // 기존 DamageInfo 전달 필요 없음 → AOEDamageActor에서 직접 CachedDamageInfo 사용
        AOEInstance->OnAOEOverlapActor.RemoveDynamic(this, &UCP_Attacks::AOEDamageActor);
        AOEInstance->OnAOEOverlapActor.AddDynamic(this, &UCP_Attacks::AOEDamageActor);

        AOEInstance->Trigger();
    }
}

void UCP_Attacks::AOEDamageActor(AActor* HitActor)
{
    if (!HitActor) return;

    // 피해를 받는 액터가 IDamageableInterface를 구현했는지 확인
    if (HitActor->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()) &&
        GetOwner()->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()))
    {
        int32 HitActorTeamNumber = IDamageableInterface::Execute_GetTeamNumber(HitActor);
        int32 OwnerTeamNumber = IDamageableInterface::Execute_GetTeamNumber(GetOwner());

        // 팀 번호가 다를 때만 피해 처리
        if (HitActorTeamNumber != OwnerTeamNumber)
        {
            IDamageableInterface::Execute_TakeDamage(HitActor, AOECachedDamageInfo, GetOwner());
        }
    }
}

void UCP_Attacks::SphereTraceDamage(float Radius, float Length, const FDamageInfo& DamageInfo)
{
    UWorld* World = GetWorld();
    if (!World) return;



    FVector StartLocation = GetOwner()->GetActorLocation();
    //UE_LOG(LogTemp, Warning, TEXT("StartLocation Spawn Location: %s"), *StartLocation.ToString());
    FVector ForwardVector = GetOwner()->GetActorForwardVector();
    FVector EndLocation = StartLocation + (ForwardVector * Length);



    // 파라매터들
    FCollisionQueryParams TraceParams(FName(TEXT("SphereTraceDamage")), false, GetOwner());
    TraceParams.bReturnPhysicalMaterial = true;

    //Sphere Trace 한거
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

    //---------------------------------------------------디버깅 SphereTrace

    ////  디버그용 라인 그리기 (트레이스 경로 확인)
    //DrawDebugLine(World, StartLocation, EndLocation, FColor::Red, false, 2.0f, 0, 2.0f);

    ////  디버그용 시작 & 끝점 스피어 그리기
    //DrawDebugSphere(World, StartLocation, Radius, 50, FColor::Blue, false, 2.0f);
    //DrawDebugSphere(World, EndLocation, Radius, 12, FColor::Green, false, 2.0f);

    ////  히트된 위치마다 스피어 그리기
    //if (bHit)
    //{
    //    for (const FHitResult& Hit : HitResults)
    //    {
    //        DrawDebugSphere(World, Hit.ImpactPoint, Radius * 2.0f, 12, FColor::Yellow, false, 10.0f);
    //        UE_LOG(LogTemp, Warning, TEXT("SphereTrace Hit: %s"), *Hit.GetActor()->GetName());
    //    }
    //}

    //--------------------------------------------------------------------------------------------------------------

    // 같은팀 공격 판별
    if (bHit)
    {
        DamageAllNonTeamMembers(HitResults, DamageInfo);
    }
}

void UCP_Attacks::JumpToAttackTarget(AActor* AttackTarget)
{
    if (!AttackTarget || !GetOwner()) return; // 유효성 검사

    // 1. 현재 위치와 타겟 위치 가져오기
    FVector StartPosition = GetOwner()->GetActorLocation();
    FVector AttackTargetPosition = AttackTarget->GetActorLocation();

    // 2. 타겟의 미래 위치 계산
    float PredictionTime = 1.0f; // 예측 시간
    FVector PredictedTargetLocation = CalculateFutureActorLocation(AttackTarget, PredictionTime);

    // Z 축을 100만큼 올림
    PredictedTargetLocation.Z += 100.0f;

    // 3. 거리 계산
    float Distance = FVector::Dist(StartPosition, AttackTargetPosition);

    // 4. 거리 범위를 Normalize (400 ~ 800 사이로 제한)
    float NormalizedRange = FMath::Clamp((Distance - 400.0f) / 400.0f, 0.0f, 1.0f);

    // 5. 속도 계산 (Lerp 사용)
    float Alpha = FMath::Lerp(0.5f, 0.94f, NormalizedRange);
    FVector LaunchVelocity;

    // 6. 포물선 속도 계산
    bool bHasValidVelocity = UGameplayStatics::SuggestProjectileVelocity_CustomArc(
        this,
        LaunchVelocity,
        StartPosition,
        PredictedTargetLocation,
        0.0f // 중력 오버라이드 값 (기본 중력 사용)
    );

    if (!bHasValidVelocity)
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to calculate projectile velocity!"));
        return;
    }

    // 7. 점프 로직
    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (OwnerCharacter)
    {
        OwnerCharacter->LaunchCharacter(LaunchVelocity, true, true);

        // 착지 이벤트 바인딩
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
        // 언바인드
        OwnerCharacter->LandedDelegate.RemoveDynamic(this, &UCP_Attacks::OnLand);

        // Stop the character's movement
        OwnerCharacter->GetCharacterMovement()->StopMovementImmediately();
    }
}

void UCP_Attacks::GroundSmash(FAttackInfo& AttackInfo, float Radius)
{
    AttackBase(AttackInfo.AttackTarget);
    ExecuteAttack_Internal(AttackInfo.Montage, "Smash", [this, Radius, DamageInfo = AttackInfo.DamageInfo](FName NotifyName)
        {
            AOEDamage(Radius, DamageInfo);
        });
}

void UCP_Attacks::PrimaryMeleeAttack(FAttackInfo& AttackInfo, float Radius, float Length)
{
    AttackBase(AttackInfo.AttackTarget);
    ExecuteAttack_Internal(AttackInfo.Montage, "Slash", [this, Radius, Length, DamageInfo = AttackInfo.DamageInfo](FName NotifyName)
        {
            SphereTraceDamage(Radius, Length, DamageInfo);
        });
}

void UCP_Attacks::LongRangeMeleeAttack(FAttackInfo& AttackInfo)
{
    AttackBase(AttackInfo.AttackTarget);
    // NAME_None을 전달하여 모든 노티파이를 람다 안에서 직접 처리
    ExecuteAttack_Internal(AttackInfo.Montage, NAME_None, [this, AttackInfo](FName ReceivedNotifyName)
        {
            if (ReceivedNotifyName == "Slash")
            {
                SphereTraceDamage(20.0f, 200.0f, AttackInfo.DamageInfo);
            }
            else if (ReceivedNotifyName == "Jump")
            {
                JumpToAttackTarget(AttackInfo.AttackTarget);
            }
        });
}

void UCP_Attacks::SpinningMeleeAttack(FAttackInfo& AttackInfo)
{
    AttackBase(AttackInfo.AttackTarget);
    ExecuteAttack_Internal(AttackInfo.Montage, NAME_None, [this, DamageInfo = AttackInfo.DamageInfo](FName ReceivedNotifyName)
        {
            if (ReceivedNotifyName == "Slash")
            {
                SphereTraceDamage(20, 200, DamageInfo);
            }
            else if (ReceivedNotifyName == "AOESlash")
            {
                AOEDamage(300.0f, DamageInfo);
            }
        });
}

void UCP_Attacks::BasicMageSpell(FAttackInfo& AttackInfo)
{
    USkeletalMeshComponent* MeshComp = AttackBase(AttackInfo.AttackTarget);
    // 그냥  NotifyName을 사용하면 람다 캡처가 불가능하므로 mutable 사용 나중에 삭제예정
	ExecuteAttack_Internal(AttackInfo.Montage, "Fire", [this, MeshComp, AttackInfo](FName NotifyName)mutable 
        {
            if (MeshComp)
            {
                FVector Location = MeshComp->GetSocketLocation("RightHand");
                FTransform SpawnTransform(FRotator::ZeroRotator, Location);
                MagicSpell(SpawnTransform, AttackInfo.AttackTarget, AttackInfo.DamageInfo);
            }
        });
}

void UCP_Attacks::BarrageMagicSpell(FAttackInfo& AttackInfo)
{
    USkeletalMeshComponent* MeshComp = AttackBase(AttackInfo.AttackTarget);
    // 그냥  NotifyName을 사용하면 람다 캡처가 불가능하므로 mutable 사용 나중에 삭제예정
    ExecuteAttack_Internal(AttackInfo.Montage, "Fire", [this, MeshComp, AttackInfo](FName NotifyName)mutable
        {
            if (MeshComp)
            {
                FVector SocketLocation = MeshComp->GetSocketLocation("RightHand");
                FVector OwnerLocation = GetOwner()->GetActorLocation();
                float RandomY = FMath::RandRange(-300.0f, 300.0f);
                FVector TargetLocation = FVector(OwnerLocation.X, OwnerLocation.Y + RandomY, OwnerLocation.Z);
                FRotator LookAtRotation = FRotationMatrix::MakeFromX(TargetLocation - SocketLocation).Rotator();
                FTransform SpawnTransform(LookAtRotation, SocketLocation);
                MagicSpell(SpawnTransform, AttackInfo.AttackTarget, AttackInfo.DamageInfo);
            }
        });
}

void UCP_Attacks::OnProjectileHit(AActor* OtherActor, const FHitResult& Hit)
{
    FDamageInfo DamageInfo=CurrentDamageInfo;
    if (OtherActor && OtherActor->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()))
    {
        // TakeDamage호출
        IDamageableInterface::Execute_TakeDamage(OtherActor,DamageInfo,GetOwner());
    }
    else
    {
        return;
    }


    UAISense_Damage::ReportDamageEvent(
        this,             // WorldContextObject
        OtherActor,     // 공격받은 액터
        GetOwner(),       // 공격한 액터
        DamageInfo.Amount,     // 데미지량
        GetOwner()->GetActorLocation(),    // 이벤트 발생 위치 (보통 Instigator 위치)
        Hit.ImpactPoint,      // 충돌 위치
        NAME_None         // 태그 (필요하면 지정 가능)
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

void UCP_Attacks::OnMontageCompleted_UseInterrupted(UAnimMontage* Montage, bool bInterrupted)
{
    // Owner가 IEnemyAIInterface를 구현했는지 확인
    if (GetOwner()->GetClass()->ImplementsInterface(UEnemyAIInterface::StaticClass()))
    {
        // AttackEnd 호출
        IEnemyAIInterface::Execute_AttackEnd(GetOwner(), CurrentAttackTarget);
    }
    if (bInterrupted == true) 
    {
        OnInterrupted(true);
    }
}

void UCP_Attacks::OnMontageCompleted_NotUseInterrupted(UAnimMontage* Montage, bool bInterrupted)
{
    // Owner가 IEnemyAIInterface를 구현했는지 확인
    if (GetOwner()->GetClass()->ImplementsInterface(UEnemyAIInterface::StaticClass()))
    {
        // AttackEnd 호출
        IEnemyAIInterface::Execute_AttackEnd(GetOwner(), CurrentAttackTarget);
    }
}

void UCP_Attacks::OnMontageBlendedOut(UAnimMontage* Montage, bool bInterrupted)
{
    if (bInterrupted)
    {
        // Owner가 IDamageableInterface를 구현했는지 확인
        if (GetOwner() && GetOwner()->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()))
        {
            // SetIsInterruptible 호출
            IDamageableInterface::Execute_SetIsInterruptible(GetOwner(), true);
        }

        // Owner가 IEnemyAIInterface를 구현했는지 확인
        if (GetOwner() && GetOwner()->GetClass()->ImplementsInterface(UEnemyAIInterface::StaticClass()))
        {
            // AttackEnd 호출
            IEnemyAIInterface::Execute_AttackEnd(GetOwner(),CurrentAttackTarget);
        }
    }
    else
    {
        // Blendout 처리
    }
}


void UCP_Attacks::OnInterrupted(bool binterruptible)
{
    if (GetOwner())
    {
        // Owner가 IDamageableInterface를 구현했는지 확인
        if (GetOwner()->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()))
        {
            // SetIsInterruptible 호출
            IDamageableInterface::Execute_SetIsInterruptible(GetOwner(), binterruptible);
        }
    }
}

void UCP_Attacks::OnMontageInterrupted(UAnimMontage* Montage, bool bInterrupted)
{
    if (GetOwner())
    {


        // Owner가 IEnemyAIInterface를 구현했는지 확인
        if (GetOwner()->GetClass()->ImplementsInterface(UEnemyAIInterface::StaticClass()))
        {
            // AttackEnd 호출
            IEnemyAIInterface::Execute_AttackEnd(GetOwner(), CurrentAttackTarget);
        }

        // Owner가 IDamageableInterface를 구현했는지 확인
        if (GetOwner()->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()))
        {
            // SetIsInterruptible 호출
            IDamageableInterface::Execute_SetIsInterruptible(GetOwner(), true);
        }
    }
}

void UCP_Attacks::ExecuteAttack_Internal(UAnimMontage* MontageToPlay, const FName& NotifyName, TFunction<void(FName)> NotifyCallback)
{
    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    UAnimInstance* AnimInstance = OwnerCharacter ? OwnerCharacter->GetMesh()->GetAnimInstance() : nullptr;
    if (!AnimInstance || !MontageToPlay || ActiveMontageInfoMap.Contains(MontageToPlay))
    {
        return;
    }

    const float MontageLength = AnimInstance->Montage_Play(MontageToPlay);
    if (MontageLength > 0.f)
    {
        IDamageableInterface::Execute_SetIsInterruptible(GetOwner(), false);

        FAttackMontageInfo Info;
        Info.NotifyName = NotifyName;
        Info.NotifyCallback = NotifyCallback;
        ActiveMontageInfoMap.Add(MontageToPlay, Info);

        FOnMontageEnded OnMontageEndedDelegate;
        OnMontageEndedDelegate.BindUObject(this, &UCP_Attacks::OnAttackMontageEnded);
        AnimInstance->Montage_SetEndDelegate(OnMontageEndedDelegate, MontageToPlay);
    }
}

void UCP_Attacks::HandleMontageNotify(FName InNotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
    USkeletalMeshComponent* MeshComp = BranchingPointPayload.SkelMeshComponent;
    const int32 MontageInstanceID = BranchingPointPayload.MontageInstanceID;

    if (MeshComp && MontageInstanceID != INDEX_NONE)
    {
        if (UAnimInstance* AnimInstance = MeshComp->GetAnimInstance())
        {
            if (FAnimMontageInstance* MontageInstance = AnimInstance->GetMontageInstanceForID(MontageInstanceID))
            {
                if (UAnimMontage* CurrentMontage = MontageInstance->Montage)
                {
                    if (FAttackMontageInfo* FoundInfo = ActiveMontageInfoMap.Find(CurrentMontage))
                    {
                        if (FoundInfo->NotifyName == NAME_None || InNotifyName == FoundInfo->NotifyName)
                        {
                            if (FoundInfo->NotifyCallback)
                            {
                                FoundInfo->NotifyCallback(InNotifyName);
                            }
                        }
                    }
                }
            }
        }
    }
}

void UCP_Attacks::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    ActiveMontageInfoMap.Remove(Montage);
    IDamageableInterface::Execute_SetIsInterruptible(GetOwner(), true);

    if (GetOwner()->GetClass()->ImplementsInterface(UEnemyAIInterface::StaticClass()))
    {
        IEnemyAIInterface::Execute_AttackEnd(GetOwner(), nullptr); // Target 정보는 콜백에서 개별 처리 필요
    }
}




