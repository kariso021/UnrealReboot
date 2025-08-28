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
	// DamageInfo ���� �� �ʱ�ȭ
	FDamageInfo DamageInfo;
	DamageInfo.Amount = 15.0f;
	DamageInfo.DamageType = EM_DamageType::Projectile;
	DamageInfo.CanBeBlocked = true;

	//AttackInfo ���� �� �ʱ�ȭ
	FAttackInfo AttackInfo;
	AttackInfo.AttackTarget = AttackTarget;
	AttackInfo.Montage = BarrageMontage;
	AttackInfo.DamageInfo = DamageInfo;


    AttackComponent->BarrageMagicSpell(AttackInfo);

}

void AEnemyMuriel::Teleport(const FVector& TargetLocation)
{
    // Branch: ���� �ڷ���Ʈ ���� �ƴ϶�� ������ �����մϴ�.
    if (!bIsTeleporting)
    {
        // SET IsTeleporting = true
        bIsTeleporting = true;

        // Character Movement ������Ʈ�� �����ɴϴ�.
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
            GetMesh()->SetVisibility(false, true); // �� ��° ���ڴ� �ڽĿ��Ե� ����(Propagate to Children)
        }

        //if (GetCapsuleComponent())
        //{
        //    // Set Collision Response to Channel (Target: Capsule Component)
        //    GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
        //}

        // --- �� ��° �̹��� ��� ���� ---

        // Spawn Emitter Attached (Teleport Body Effect)
        // ����Ʈ ������ ����� �����Ǿ����� Ȯ��
        if (TeleportBodyEffect)
        {
            // FName("None_01")�� ���� �̸��� �� ������ Ȯ���� �ʿ��մϴ�.
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
        // ���� ��� ���� ��� ��Ÿ�ָ� �����մϴ�.
        StopAnimMontage();

        // AI Move To
        // �� ĳ���͸� �����ϴ� AI ��Ʈ�ѷ��� �����ɴϴ�.
        //AAIController* AIController = Cast<AAIController>(GetController());
        //if (AIController)
        //{
        //    // ��ǥ �������� �̵��� ����մϴ�.
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
