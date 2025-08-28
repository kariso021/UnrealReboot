#include "Enemies/Muriel/EnemyMuriel.h"

AEnemyMuriel::AEnemyMuriel()
{
	// Initialization codehere
}

void AEnemyMuriel::BeginPlay()
{
	Super::BeginPlay();
	// Muriel-specific initialization or begin play logic
}

void AEnemyMuriel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Muriel-specific tick code
}

void AEnemyMuriel::BasicAttack()
{
}

void AEnemyMuriel::GroundSmashAttack()
{
}

void AEnemyMuriel::BarrageAttack(AActor* AttackTarget)
{
	// DamageInfo 생성 및 초기화
	FDamageInfo DamageInfo;
	DamageInfo.Amount = 15.0f;
	DamageInfo.DamageType = EM_DamageType::Projectile;
	DamageInfo.CanBeBlocked = true;

	//AttackInfo 생성 및 초기화
	FAttackInfo AttackInfo;
	AttackInfo.AttackTarget = AttackTarget;
	AttackInfo.Montage = BarrageMontage;
	AttackInfo.DamageInfo = DamageInfo;


    AttackComponent->BarrageMagicSpell(AttackInfo);

}

void AEnemyMuriel::Teleport(const FVector& TargetLocation)
{
    // Branch: 현재 텔레포트 중이 아니라면 로직을 실행합니다.
    if (!bIsTeleporting)
    {
        // SET IsTeleporting = true
        bIsTeleporting = true;

        // Character Movement 컴포넌트를 가져옵니다.
        if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
        {
            // Set Movement Mode = Flying
            MoveComp->SetMovementMode(EMovementMode::MOVE_Flying);

            // SET Max 
            MoveComp->MaxFlySpeed = 50000.0f;

            // SET Max
            MoveComp->MaxAcceleration = 200000.0f;
        }

        if (GetMesh())
        {
            // SET Visibility (Target: Mesh)
            GetMesh()->SetVisibility(false, true); // 두 번째 인자는 자식에게도 적용(Propagate to Children)
        }

        //if (GetCapsuleComponent())
        //{
        //    // Set Collision Response to Channel (Target: Capsule Component)
        //    GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
        //}

        // --- 두 번째 이미지 노드 시작 ---

        // Spawn Emitter Attached (Teleport Body Effect)
        // 이펙트 에셋이 제대로 설정되었는지 확인
        if (TeleportBodyEffect)
        {
            // FName("None_01")은 소켓 이름일 수 있으니 확인이 필요합니다.
            SpawnedBodyEffectComponent = UGameplayStatics::SpawnEmitterAttached(
                TeleportBodyEffect,
                GetMesh(),
                FName("None_01"), // Attach Point Name
                FVector::ZeroVector,
                FRotator::ZeroRotator,
                FVector::OneVector,
                EAttachLocation::KeepRelativeOffset,
                true // Auto Destroy
            );
        }

        // Spawn Emitter Attached (Teleport Trail Effect)
        if (TeleportTrailEffect)
        {
            SpawnedTrailEffectComponent = UGameplayStatics::SpawnEmitterAttached(
                TeleportTrailEffect,
                GetMesh(),
                FName("None_01"), // Attach Point Name
                FVector::ZeroVector,
                FRotator::ZeroRotator,
                FVector::OneVector,
                EAttachLocation::KeepRelativeOffset,
                true // Auto Destroy
            );
        }

        // Stop Anim Montage
        // 현재 재생 중인 모든 몽타주를 중지합니다.
        StopAnimMontage();

        // AI Move To
        // 이 캐릭터를 조종하는 AI 컨트롤러를 가져옵니다.
        //AAIController* AIController = Cast<AAIController>(GetController());
        //if (AIController)
        //{
        //    // 목표 지점으로 이동을 명령합니다.
        //    AIController->MoveToLocation(
        //        TargetLocation,
        //        5.0f, // Acceptance Radius
        //        false  // Stop on Overlap
        //    );
        //}
    }
}

void AEnemyMuriel::TeleportEnd()
{
	// Logic to execute after teleportation
}

void AEnemyMuriel::StartHealOverTime(float HealAmountPerTick, float TickInterval, float Duration)
{
}

void AEnemyMuriel::StopHealOverTime()
{
}
