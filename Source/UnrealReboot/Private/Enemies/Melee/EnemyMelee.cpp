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
	UE_LOG(LogTemp, Display, TEXT("GroundSmashExecuted"));
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
	AttackInfo.Montage = GroundSmashMontage; // 애니메이션 몽타주 설정 (필요하면 추가 가능)
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
	AttackInfo.Montage = SwordSlashMontage; // 애니메이션 몽타주 설정 (필요하면 추가 가능)
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
	AttackInfo.Montage = SwordJumpAttackMontage; // 애니메이션 몽타주 설정 (필요하면 추가 가능)
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
	AttackInfo.Montage = SwordSpinningAttackMontage; // 애니메이션 몽타주 설정 (필요하면 추가 가능)
	AttackInfo.DamageInfo = DamageInfo;

	// AttackComponent를 통해 Ground Smash 실행
	AttackComponent->SpinningMeleeAttack(AttackInfo);
}


//나중에 두개 다 만들자
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

			// 플레이 성공적일시
			float MontageLength = AnimInstance->Montage_Play(UnSheathSwordMontage, MontagePlayRate, EMontagePlayReturnType::MontageLength, MontageStartPosition);
			bool bPlayedSuccessfully = MontageLength > 0.f;

			if (bPlayedSuccessfully)
			{

				// EndDelegtate 여기서 바인드
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

			// 플레이 성공적일시
			float MontageLength = AnimInstance->Montage_Play(SheathSwordMontage, MontagePlayRate, EMontagePlayReturnType::MontageLength, MontageStartPosition);
			bool bPlayedSuccessfully = MontageLength > 0.f;

			if (bPlayedSuccessfully)
			{

				// EndDelegtate 여기서 바인드
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
		if (WeaponBlueprint) // Blueprint 클래스가 설정되어 있는지 확인
		{
			// 무기 생성
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this; // 소유자를 설정
			SpawnParams.Instigator = GetInstigator();

			WeaponActor = GetWorld()->SpawnActor<AActor>(WeaponBlueprint, GetActorTransform(), SpawnParams);
			if (WeaponActor)
			{
				// 소켓에 무기 부착
				FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
				WeaponActor->AttachToComponent(GetMesh(), AttachmentRules, FName("hand_r_sword_socket"));

				// 상태 업데이트
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
			// 생성된 액터 삭제
			WeaponActor->Destroy();

			// 상태 업데이트
			IsWieldingWeapon = false;

			// 포인터 초기화
			WeaponActor = nullptr;

			UE_LOG(LogTemp, Log, TEXT("Weapon destroyed."));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No weapon to destroy."));
		}
	}
}


