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
	UE_LOG(LogTemp, Display, TEXT("GroundSmashExecuted"));
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
	AttackInfo.Montage = GroundSmashMontage; // �ִϸ��̼� ��Ÿ�� ���� (�ʿ��ϸ� �߰� ����)
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
	AttackInfo.Montage = SwordSlashMontage; // �ִϸ��̼� ��Ÿ�� ���� (�ʿ��ϸ� �߰� ����)
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
	AttackInfo.Montage = SwordJumpAttackMontage; // �ִϸ��̼� ��Ÿ�� ���� (�ʿ��ϸ� �߰� ����)
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
	AttackInfo.Montage = SwordSpinningAttackMontage; // �ִϸ��̼� ��Ÿ�� ���� (�ʿ��ϸ� �߰� ����)
	AttackInfo.DamageInfo = DamageInfo;

	// AttackComponent�� ���� Ground Smash ����
	AttackComponent->SpinningMeleeAttack(AttackInfo);
}


//���߿� �ΰ� �� ������
void AEnemyMelee::EquipWeapon()
{
	if (GetMesh() && UnSheathSwordMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		if (AnimInstance)
		{
			float MontagePlayRate = 1.0f;
			float MontageStartPosition = 0.0f;
			FName MontageStartingSection = NAME_None;

			// �÷��� �������Ͻ�
			float MontageLength = AnimInstance->Montage_Play(UnSheathSwordMontage, MontagePlayRate, EMontagePlayReturnType::MontageLength, MontageStartPosition);
			bool bPlayedSuccessfully = MontageLength > 0.f;

			if (bPlayedSuccessfully)
			{

				// EndDelegtate ���⼭ ���ε�
				FOnMontageEnded OnMontageEndedDelegate;
				OnMontageEndedDelegate.BindUObject(this, &AEnemyMelee::UnSheath_MontageCompleted);



				AnimInstance->Montage_SetEndDelegate(OnMontageEndedDelegate, UnSheathSwordMontage);

				// Bind to Notify Begin and End for additional actions
				AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &AEnemyMelee::OnNotifyBeginReceived_EquipWeapon);

			}
		}



	}
}

void AEnemyMelee::UnEquipWeapon()
{
	if (GetMesh() && SheathSwordMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		if (AnimInstance)
		{
			float MontagePlayRate = 1.0f;
			float MontageStartPosition = 0.0f;
			FName MontageStartingSection = NAME_None;

			// �÷��� �������Ͻ�
			float MontageLength = AnimInstance->Montage_Play(SheathSwordMontage, MontagePlayRate, EMontagePlayReturnType::MontageLength, MontageStartPosition);
			bool bPlayedSuccessfully = MontageLength > 0.f;

			if (bPlayedSuccessfully)
			{

				// EndDelegtate ���⼭ ���ε�
				FOnMontageEnded OnMontageEndedDelegate;
				OnMontageEndedDelegate.BindUObject(this, &AEnemyMelee::Sheath_MontageCompleted);



				AnimInstance->Montage_SetEndDelegate(OnMontageEndedDelegate, SheathSwordMontage);

				// Bind to Notify Begin and End for additional actions
				AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &AEnemyMelee::OnNotifyBeginReceived_UnEquipWeapon);

			}
			else {

			}
		}



	}
}


//----------------------Block

void AEnemyMelee::BlockTrigger()
{
}

//-------------------------------------------MontageFunction

void AEnemyMelee::UnSheath_MontageCompleted(UAnimMontage* Montage, bool bInterrupted)
{
	OnWeaponEquipped.Broadcast();
}

void AEnemyMelee::Sheath_MontageCompleted(UAnimMontage* Montage, bool bInterrupted)
{
	OnWeaponUnEquipped.Broadcast();
}

void AEnemyMelee::OnNotifyBeginReceived_EquipWeapon(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if (NotifyName == "HoldSword")
	{
		if (WeaponBlueprint) // Blueprint Ŭ������ �����Ǿ� �ִ��� Ȯ��
		{
			// ���� ����
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this; // �����ڸ� ����
			SpawnParams.Instigator = GetInstigator();

			WeaponActor = GetWorld()->SpawnActor<AActor>(WeaponBlueprint, GetActorTransform(), SpawnParams);
			if (WeaponActor)
			{
				// ���Ͽ� ���� ����
				FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
				WeaponActor->AttachToComponent(GetMesh(), AttachmentRules, FName("hand_r_sword_socket"));

				// ���� ������Ʈ
				IsWieldingWeapon = true;

				UE_LOG(LogTemp, Log, TEXT("Weapon spawned and attached."));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("WeaponBlueprint is not set!"));
		}
	}
}

void AEnemyMelee::OnNotifyBeginReceived_UnEquipWeapon(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if (NotifyName == "DropSword")
	{
		if (WeaponActor)
		{
			// ������ ���� ����
			WeaponActor->Destroy();

			// ���� ������Ʈ
			IsWieldingWeapon = false;

			// ������ �ʱ�ȭ
			WeaponActor = nullptr;

			UE_LOG(LogTemp, Log, TEXT("Weapon destroyed."));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No weapon to destroy."));
		}
	}
}


