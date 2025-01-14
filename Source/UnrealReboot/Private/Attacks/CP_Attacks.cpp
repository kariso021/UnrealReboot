// Fill out your copyright notice in the Description page of Project Settings.


#include "Attacks/CP_Attacks.h"
#include "DrawDebugHelpers.h"//라인트레이스 디버깅용 인클루드 함수
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
                    IDamageableInterface::Execute_TakeDamage(HitActor, DamageInfo, GetOwner());
                    ActorsDamagedSoFar.AddUnique(HitActor);
                }
            }
        }
    }

    // 손상된 액터 목록 반환
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
            // 매핑 추가
            DamageInfoMappings.Add(FDamageInfoMapping(Projectile, DamageInfo));

            // 프로젝타일의 속도 설정

            // 이벤트 핸들러 설정
            Projectile->OnProjectileImpact.AddDynamic(this, &UCP_Attacks::OnProjectileHit);

            // 블루프린트에서 보여준 'Ignore Actor when Moving' 노드에 해당하는 부분
            UPrimitiveComponent* ProjectileComponent = Projectile->FindComponentByClass<UPrimitiveComponent>();
            if (ProjectileComponent != nullptr)
            {
                // 소유자를 무시하도록 설정
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
    //DamageInfoMapping 이 충돌난다는 것을 앎-> 그러나 같이 쓰일일이 없기때문에 그대로 InfoMapping 그대로 씀
    
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

        // Actor와 Owner가 IDamageableInterface를 구현했는지 확인
        if (Actor->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()) &&
            GetOwner()->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()))
        {
            // 인터페이스 함수 호출
            int32 ActorTeamNumber = IDamageableInterface::Execute_GetTeamNumber(Actor);
            int32 OwnerTeamNumber = IDamageableInterface::Execute_GetTeamNumber(GetOwner());

            // 팀 번호가 다른 경우 피해 처리
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

    // 같은팀 공격 판별
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
        FVector EndPos = CalculateFutureActorLocation(AttackTarget, 1.0f); // 1초 동안

        FVector LaunchVelocity;
        if (UGameplayStatics::SuggestProjectileVelocity_CustomArc(
            this,
            LaunchVelocity,
            StartPos,
            EndPos,
            100.0f))
        {
            OwnerCharacter->LaunchCharacter(LaunchVelocity, true, true);

            // Land 할때 바인드
            OwnerCharacter->LandedDelegate.AddDynamic(this,&UCP_Attacks::OnLand);
        }
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
    CurrentAttackInfo = AttackInfo;
    CurrentRadius = Radius;

    USkeletalMeshComponent* MeshComp = AttackBase(AttackInfo.AttackTarget);// 매쉬컴포넌트 받아오기
    UAnimMontage* MontageToPlay = AttackInfo.Montage;

    if (MeshComp && MontageToPlay)
    {
        UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
        if (AnimInstance)
        {
            float MontagePlayRate = 1.0f;
            float MontageStartPosition = 0.0f;
            FName MontageStartingSection= NAME_None; 

            // 플레이 성공적일시
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

                // EndDelegtate 여기서 바인드
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

    USkeletalMeshComponent* MeshComp = AttackBase(AttackInfo.AttackTarget); // 매쉬 컴포넌트 받아오기
    UAnimMontage* MontageToPlay = AttackInfo.Montage;

    if (MeshComp && MontageToPlay)
    {
        UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
        if (AnimInstance)
        {
            float MontagePlayRate = 1.0f;
            float MontageStartPosition = 0.0f;
            FName MontageStartingSection = NAME_None;

            // 애니메이션 재생
            float MontageLength = AnimInstance->Montage_Play(MontageToPlay, MontagePlayRate, EMontagePlayReturnType::MontageLength, MontageStartPosition);
            bool bPlayedSuccessfully = MontageLength > 0.f;

            // Owner가 DamageableInterface를 구현했는지 확인
            if (GetOwner() && GetOwner()->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()))
            {
                // 인터페이스 함수 호출
                IDamageableInterface::Execute_SetIsInterruptible(GetOwner(), false);
            }

            if (bPlayedSuccessfully)
            {
                // 특정 섹션으로 이동
                if (MontageStartingSection != NAME_None)
                {
                    AnimInstance->Montage_JumpToSection(MontageStartingSection, MontageToPlay);
                }

                // EndDelegate 바인드
                FOnMontageEnded OnMontageEndedDelegate;
                OnMontageEndedDelegate.BindUObject(this, &UCP_Attacks::OnMontageCompleted);

                AnimInstance->Montage_SetEndDelegate(OnMontageEndedDelegate, MontageToPlay);

                // Notify Begin 바인드
                AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &UCP_Attacks::OnNotifyBeginReceived_GroundSmash);
            }
            else
            {
                // 몽타주 재생 실패 처리
                OnInterrupted();
            }
        }
    }
}

void UCP_Attacks::LongRangeMeleeAttack(FAttackInfo& AttackInfo)
{
    CurrentAttackInfo = AttackInfo;

    USkeletalMeshComponent* MeshComp = AttackBase(AttackInfo.AttackTarget); // 매쉬 컴포넌트 받아오기
    UAnimMontage* MontageToPlay = AttackInfo.Montage;

    if (MeshComp && MontageToPlay)
    {
        UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
        if (AnimInstance)
        {
            float MontagePlayRate = 1.0f;
            float MontageStartPosition = 0.0f;
            FName MontageStartingSection = NAME_None; // 필요한 경우 설정

            // 몽타주 재생 및 성공 여부 확인
            float MontageLength = AnimInstance->Montage_Play(MontageToPlay, MontagePlayRate, EMontagePlayReturnType::MontageLength, MontageStartPosition);
            bool bPlayedSuccessfully = MontageLength > 0.f;

            // Owner가 IDamageableInterface를 구현했는지 확인
            if (GetOwner() && GetOwner()->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()))
            {
                // SetIsInterruptible 호출
                IDamageableInterface::Execute_SetIsInterruptible(GetOwner(), false);
            }

            if (bPlayedSuccessfully)
            {
                // 필요한 경우 특정 섹션으로 이동
                if (MontageStartingSection != NAME_None)
                {
                    AnimInstance->Montage_JumpToSection(MontageStartingSection, MontageToPlay);
                }

                // OnMontageEnded 델리게이트 바인딩
                FOnMontageEnded OnMontageEndedDelegate;
                OnMontageEndedDelegate.BindUObject(this, &UCP_Attacks::OnMontageCompleted);
                AnimInstance->Montage_SetEndDelegate(OnMontageEndedDelegate, MontageToPlay);

                // Notify Begin 이벤트 바인딩
                AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &UCP_Attacks::OnNotifyBeginReceived_LongRangeMeleeAttack);
            }
            else
            {
                // 몽타주 재생 실패 또는 중단 처리
                OnInterrupted();
            }
        }
    }
}

void UCP_Attacks::SpinningMeleeAttack(FAttackInfo& AttackInfo)
{
    CurrentAttackInfo = AttackInfo;

    USkeletalMeshComponent* MeshComp = AttackBase(AttackInfo.AttackTarget); // 매쉬 컴포넌트 받아오기
    UAnimMontage* MontageToPlay = AttackInfo.Montage;

    if (MeshComp && MontageToPlay)
    {
        UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
        if (AnimInstance)
        {
            float MontagePlayRate = 1.0f;
            float MontageStartPosition = 0.0f;
            FName MontageStartingSection = NAME_None; // 필요한 경우 설정

            // 몽타주 재생 및 성공 여부 확인
            float MontageLength = AnimInstance->Montage_Play(MontageToPlay, MontagePlayRate, EMontagePlayReturnType::MontageLength, MontageStartPosition);
            bool bPlayedSuccessfully = MontageLength > 0.f;

            // Owner가 IDamageableInterface를 구현했는지 확인
            if (GetOwner() && GetOwner()->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()))
            {
                // SetIsInterruptible 호출
                IDamageableInterface::Execute_SetIsInterruptible(GetOwner(), false);
            }

            if (bPlayedSuccessfully)
            {
                // 필요한 경우 특정 섹션으로 이동
                if (MontageStartingSection != NAME_None)
                {
                    AnimInstance->Montage_JumpToSection(MontageStartingSection, MontageToPlay);
                }

                // OnMontageEnded 델리게이트 바인딩
                FOnMontageEnded OnMontageEndedDelegate;
                OnMontageEndedDelegate.BindUObject(this, &UCP_Attacks::OnMontageCompleted);
                AnimInstance->Montage_SetEndDelegate(OnMontageEndedDelegate, MontageToPlay);

                // Notify Begin 이벤트 바인딩
                AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &UCP_Attacks::OnNotifyBeginReceived_SpinningMeleeAttack);
            }
            else
            {
                // 몽타주 재생 실패 또는 중단 처리
                OnInterrupted();
            }
        }
    }
}

void UCP_Attacks::BasicMageSpell(FAttackInfo& AttackInfo)
{
    CurrentAttackInfo = AttackInfo;

    USkeletalMeshComponent* MeshComp = AttackBase(AttackInfo.AttackTarget);// 매쉬컴포넌트 받아오기
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

    USkeletalMeshComponent* MeshComp = AttackBase(AttackInfo.AttackTarget);// 매쉬컴포넌트 받아오기
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
            // 데미지 정보 사용
            FDamageInfo& DamageInfo = DamageInfoMappings[Index].DamageInfo;
            // 데미지 처리 로직...

            // 사용 후 매핑 제거
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
    //Interrupted 를 여기다가 쓸수 있는데, 어차피 되든 안되든 똑같이 OnInterrupted 사용할것임
    //  분기문을 통해서 true일때 complete 아닐때는 interrupted 임
    OnInterrupted();
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




