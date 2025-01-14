#include "Enemies/Melee/EnemyMelee.h"

AEnemyMelee::AEnemyMelee()
{
	// MeshRotationBeforeSpin �ʱ�ȭ
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

	// GetMesh()�� ����Ͽ� Mesh�� ����� ȸ������ ������ ����
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
	// DamageInfo ���� �� �ʱ�ȭ
	FDamageInfo DamageInfo;
	DamageInfo.Amount = 25.0f;
	DamageInfo.DamageType = EM_DamageType::Explosion;
	DamageInfo.CanBeBlocked = false;
	DamageInfo.CanBeParried = false;
	DamageInfo.ShouldForceInterrupt = false;

	// AttackInfo ���� �� �ʱ�ȭ
	FAttackInfo AttackInfo;
	AttackInfo.AttackTarget = AttackTarget;
	AttackInfo.Montage = nullptr; // �ִϸ��̼� ��Ÿ�� ���� (�ʿ��ϸ� �߰� ����)
	AttackInfo.DamageInfo = DamageInfo;

	// AttackComponent�� ���� Ground Smash ����
	AttackComponent->GroundSmash(AttackInfo, 300.0f);

}

void AEnemyMelee::ShortRangeAttack(AActor* AttackTarget)
{
	// DamageInfo ���� �� �ʱ�ȭ
	FDamageInfo DamageInfo;
	DamageInfo.Amount = 10.0f;
	DamageInfo.DamageType = EM_DamageType::Melee;
	DamageInfo.CanBeBlocked = true;
	DamageInfo.CanBeParried = true;
	DamageInfo.ShouldForceInterrupt = false;

	// AttackInfo ���� �� �ʱ�ȭ
	FAttackInfo AttackInfo;
	AttackInfo.AttackTarget = AttackTarget;
	AttackInfo.Montage = nullptr; // �ִϸ��̼� ��Ÿ�� ���� (�ʿ��ϸ� �߰� ����)
	AttackInfo.DamageInfo = DamageInfo;

	// AttackComponent�� ���� Ground Smash ����
	AttackComponent->PrimaryMeleeAttack(AttackInfo, 20.0f,200.0f);


}

void AEnemyMelee::LongRangeAttack(AActor* AttackTarget)
{
	// DamageInfo ���� �� �ʱ�ȭ
	FDamageInfo DamageInfo;
	DamageInfo.Amount = 10.0f;
	DamageInfo.DamageType = EM_DamageType::Melee;
	DamageInfo.CanBeBlocked = true;
	DamageInfo.CanBeParried = true;
	DamageInfo.ShouldForceInterrupt = false;

	// AttackInfo ���� �� �ʱ�ȭ
	FAttackInfo AttackInfo;
	AttackInfo.AttackTarget = AttackTarget;
	AttackInfo.Montage = nullptr; // �ִϸ��̼� ��Ÿ�� ���� (�ʿ��ϸ� �߰� ����)
	AttackInfo.DamageInfo = DamageInfo;

	// AttackComponent�� ���� Ground Smash ����
	AttackComponent->LongRangeMeleeAttack(AttackInfo);

}

void AEnemyMelee::SpinningAttack(AActor* AttackTarget)
{

	// DamageInfo ���� �� �ʱ�ȭ
	FDamageInfo DamageInfo;
	DamageInfo.Amount = 10.0f;
	DamageInfo.DamageType = EM_DamageType::Melee;
	DamageInfo.CanBeBlocked = true;
	DamageInfo.CanBeParried = true;
	DamageInfo.ShouldForceInterrupt = false;

	// AttackInfo ���� �� �ʱ�ȭ
	FAttackInfo AttackInfo;
	AttackInfo.AttackTarget = AttackTarget;
	AttackInfo.Montage = nullptr; // �ִϸ��̼� ��Ÿ�� ���� (�ʿ��ϸ� �߰� ����)
	AttackInfo.DamageInfo = DamageInfo;

	// AttackComponent�� ���� Ground Smash ����
	AttackComponent->SpinningMeleeAttack(AttackInfo);
}


//���߿� �ΰ� �� ������
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


