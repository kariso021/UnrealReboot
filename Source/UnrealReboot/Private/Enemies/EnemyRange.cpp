#include "Enemies/EnemyRange.h"

AEnemyRange::AEnemyRange()
{
	// Initialization codehere
	tempAttackTarget = nullptr;
}

void AEnemyRange::BeginPlay()
{
	Super::BeginPlay();
	// Range-specific initialization or begin play logic
}

float AEnemyRange::SetmovementSpeed(EM_MovementSpeed Speed)
{
	switch (Speed)
	{
	case EM_MovementSpeed::Idle:
		GetCharacterMovement()->MaxWalkSpeed = 0.0f;
		return 0.0f;
		break;
	case EM_MovementSpeed::Walk:
		GetCharacterMovement()->MaxWalkSpeed = 200.0f;
		return 200.0f;
		break;
	case EM_MovementSpeed::Jogging:
		// Jogging speed is set to 300.0f
		GetCharacterMovement()->MaxWalkSpeed = 300.0f;
			return 300.0f;
		break;
	case EM_MovementSpeed::Sprinting:
		// Sprinting speed is set to 500.0f
		GetCharacterMovement()->MaxWalkSpeed = 500.0f;
		return 500.0f;
		break;
	default:
		return 0.0f; // Default case, no speed set
		break;
	}
}



void AEnemyRange::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Range-specific tick code
}

float AEnemyRange::GetAttackRadius_Implementation()
{
	return 600.0f;
}

float AEnemyRange::GetDefendRadius_Implementation()
{
	return 600.0f;
}

void AEnemyRange::EquipWeapon_Implementation()
{
	if (!WeaponClass) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	// 무기 액터 스폰
	WeaponActor = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass, FTransform::Identity, SpawnParams);
	if (!WeaponActor) return;

	// 캐릭터 Mesh의 소켓에 붙임
	WeaponActor->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("hand_r_rifle_socket"));

	IsWieldingWeapon = true; // 무기를 들고 있는 상태로 설정

	OnWeaponEquipped.Broadcast();
}

void AEnemyRange::Attack_Implementation(AActor* AttackTarget)
{
	Super::Attack_Implementation(AttackTarget);

	UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();
	if (!AnimInst || !RifleFireMontage) return;

	tempAttackTarget = AttackTarget;

	// 1) 몽타주 종료 델리게이트 등록
	AnimInst->OnMontageEnded.AddDynamic(this, &AEnemyRange::Fire_MontageCompleted);

	// 2) Notify 델리게이트 등록
	AnimInst->OnPlayMontageNotifyBegin.AddDynamic(this, &AEnemyRange::OnNotifyBeginReceived_EquipWeapon);

	// 3) 몽타주 재생
	AnimInst->Montage_Play(RifleFireMontage, 1.0f);
}

void AEnemyRange::OnNotifyBeginReceived_EquipWeapon(FName NotifyName, const FBranchingPointNotifyPayload&)
{
	if (NotifyName != TEXT("Fire") || !WeaponActor || !tempAttackTarget)
		return;

	FDamageInfo Info;
	Info.Amount = 10.f;
	Info.DamageType = EM_DamageType::Projectile;
	Info.CanBeBlocked = false;
	Info.CanBeParried = false;
	Info.ShouldForceInterrupt = false;

	AttackComponent->FireBullet(
		WeaponActor->GetActorLocation(),
		tempAttackTarget->GetActorLocation(),
		Info);
}

void AEnemyRange::Fire_MontageCompleted(UAnimMontage* Montage, bool /*bInterrupted*/)
{
	// --- 항상 호출 ---
	if (tempAttackTarget &&
		tempAttackTarget->GetClass()->ImplementsInterface(UEnemyAIInterface::StaticClass()))
	{
		IEnemyAIInterface::Execute_AttackEnd(this, tempAttackTarget);
	}

	// --- 델리게이트 해제 ---
	if (UAnimInstance* AnimInst = GetMesh()->GetAnimInstance())
	{
		AnimInst->OnPlayMontageNotifyBegin.RemoveDynamic(this, &AEnemyRange::OnNotifyBeginReceived_EquipWeapon);
		AnimInst->OnMontageEnded.RemoveDynamic(this, &AEnemyRange::Fire_MontageCompleted);
	}
}


