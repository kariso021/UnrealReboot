#include "Enemies/Melee/EnemyMelee.h"

AEnemyMelee::AEnemyMelee()
{
	// MeshRotationBeforeSpin 초기화
	MeshRotationBeforeSpin = FRotator::ZeroRotator;
}

void AEnemyMelee::BeginPlay()
{
	Super::BeginPlay();
	// melee-specific initialization or begin play logic
}

void AEnemyMelee::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// GetMesh()를 사용하여 Mesh의 상대적 회전값을 가져와 저장
	if (GetMesh())
	{
		MeshRotationBeforeSpin = GetMesh()->GetRelativeRotation();
	}
}

void AEnemyMelee::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// melee-specific tick code
}


void AEnemyMelee::GroundSmashAttack(AActor* AttackTarget)
{
	// DamageInfo 생성 및 초기화
	FDamageInfo DamageInfo;
	DamageInfo.Amount = 25.0f;
	DamageInfo.DamageType = EM_DamageType::Explosion;
	DamageInfo.CanBeBlocked = false;
	DamageInfo.CanBeParried = false;
	DamageInfo.ShouldForceInterrupt = false;

	// AttackInfo 생성 및 초기화
	FAttackInfo AttackInfo;
	AttackInfo.AttackTarget = AttackTarget;
	AttackInfo.Montage = nullptr; // 애니메이션 몽타주 설정 (필요하면 추가 가능)
	AttackInfo.DamageInfo = DamageInfo;

	// AttackComponent를 통해 Ground Smash 실행
	AttackComponent->GroundSmash(AttackInfo, 300.0f);

}

void AEnemyMelee::ShortRangeAttack(AActor* AttackTarget)
{
	// DamageInfo 생성 및 초기화
	FDamageInfo DamageInfo;
	DamageInfo.Amount = 10.0f;
	DamageInfo.DamageType = EM_DamageType::Melee;
	DamageInfo.CanBeBlocked = true;
	DamageInfo.CanBeParried = true;
	DamageInfo.ShouldForceInterrupt = false;

	// AttackInfo 생성 및 초기화
	FAttackInfo AttackInfo;
	AttackInfo.AttackTarget = AttackTarget;
	AttackInfo.Montage = nullptr; // 애니메이션 몽타주 설정 (필요하면 추가 가능)
	AttackInfo.DamageInfo = DamageInfo;

	// AttackComponent를 통해 Ground Smash 실행
	AttackComponent->PrimaryMeleeAttack(AttackInfo, 20.0f,200.0f);


}

void AEnemyMelee::LongRangeAttack(AActor* AttackTarget)
{
	// DamageInfo 생성 및 초기화
	FDamageInfo DamageInfo;
	DamageInfo.Amount = 10.0f;
	DamageInfo.DamageType = EM_DamageType::Melee;
	DamageInfo.CanBeBlocked = true;
	DamageInfo.CanBeParried = true;
	DamageInfo.ShouldForceInterrupt = false;

	// AttackInfo 생성 및 초기화
	FAttackInfo AttackInfo;
	AttackInfo.AttackTarget = AttackTarget;
	AttackInfo.Montage = nullptr; // 애니메이션 몽타주 설정 (필요하면 추가 가능)
	AttackInfo.DamageInfo = DamageInfo;

	// AttackComponent를 통해 Ground Smash 실행
	AttackComponent->LongRangeMeleeAttack(AttackInfo);

}

void AEnemyMelee::SpinningAttack(AActor* AttackTarget)
{

	// DamageInfo 생성 및 초기화
	FDamageInfo DamageInfo;
	DamageInfo.Amount = 10.0f;
	DamageInfo.DamageType = EM_DamageType::Melee;
	DamageInfo.CanBeBlocked = true;
	DamageInfo.CanBeParried = true;
	DamageInfo.ShouldForceInterrupt = false;

	// AttackInfo 생성 및 초기화
	FAttackInfo AttackInfo;
	AttackInfo.AttackTarget = AttackTarget;
	AttackInfo.Montage = nullptr; // 애니메이션 몽타주 설정 (필요하면 추가 가능)
	AttackInfo.DamageInfo = DamageInfo;

	// AttackComponent를 통해 Ground Smash 실행
	AttackComponent->SpinningMeleeAttack(AttackInfo);
}


//나중에 두개 다 만들자
void AEnemyMelee::EquipWeapon()
{
}

void AEnemyMelee::UnEquipWeapon()
{
}


//----------------------Block

void AEnemyMelee::BlockTrigger()
{
}


