#include "Enemies/Bose/EnemyBose.h"

AEnemyBoss::AEnemyBoss()
{
	// Initialization codehere
}

void AEnemyBoss::BeginPlay()
{
	Super::BeginPlay();
	// Boss-specific initialization or begin play logic
}

void AEnemyBoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Boss-specific tick code
}





//----------------------------------------Teleport-----------------------------------------

void AEnemyBoss::Teleport(FVector Location, AActor* AttackTarget)
{

}


void AEnemyBoss::TeleportEnd()
{

}


//--------------------------------------------AttackOfBose-------------------------------------

void AEnemyBoss::AttackCombo1(AActor* AttackTarget)
{
	FAttackInfo AttackInfo;
	FDamageInfo DamageInfo;
	
	DamageInfo.Amount = 20;
	DamageInfo.DamageType = EM_DamageType::Melee;
	AttackInfo.AttackTarget = AttackTarget;
	AttackInfo.DamageInfo = DamageInfo;
	AttackInfo.Montage = Combo1Montage;


	AttackComponent->PrimaryMeleeAttack(AttackInfo, 40, 220);
}

void AEnemyBoss::AttackCombo2(AActor* AttackTarget)
{
	currentAttackTarget = AttackTarget;
	if (MeshComponent && Combo2Montage)
	{
		UAnimInstance* AnimInstance = MeshComponent->GetAnimInstance();
		if (AnimInstance)
		{
			float MontagePlayRate = 1.0f;
			float MontageStartPosition = 0.0f;
			FName MontageStartingSection = NAME_None;

			// 플레이 성공적일시
			float MontageLength = AnimInstance->Montage_Play(Combo2Montage, MontagePlayRate, EMontagePlayReturnType::MontageLength, MontageStartPosition);
			bool bPlayedSuccessfully = MontageLength > 0.f;

			if (bPlayedSuccessfully)
			{
				// Optionally jump to a starting section
				if (MontageStartingSection != NAME_None)
				{
					AnimInstance->Montage_JumpToSection(MontageStartingSection, Combo2Montage);
				}

				// EndDelegtate 여기서 바인드
				FOnMontageEnded OnMontageEndedDelegate;
				OnMontageEndedDelegate.BindUObject(this, &AEnemyBoss::OnMontageCompleted);



				AnimInstance->Montage_SetEndDelegate(OnMontageEndedDelegate, Combo2Montage);

				// Bind to Notify Begin and End for additional actions
				AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &AEnemyBoss::OnNotifyBeginReceived_Combo2);

			}
			else
			{
				// Handle montage interrupted or failed to play
				OnInterrupted();
			}

		}
	}
}
void AEnemyBoss::JumpAttack(AActor* AttackTarget)
{

}

void AEnemyBoss::AOETrail(AActor* AttackTarget)
{

}

void AEnemyBoss::GroundSmash(AActor* AttackTarget)
{

}

void AEnemyBoss::MissileLaunch(AActor* AttackTarget)
{

}

void AEnemyBoss::OnInterrupted()
{
	//AttackEnd가 들어가야함
}

void AEnemyBoss::OnMontageCompleted(UAnimMontage* Montage, bool bInterrupted)
{
	//여기도 AttackEnd가 들어가야함
}

void AEnemyBoss::OnNotifyBeginReceived_Combo2(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	FDamageInfo AOESlashInfo;
	AOESlashInfo.Amount = 20;
	AOESlashInfo.DamageType = EM_DamageType::Explosion;
	FDamageInfo SlashInfo;
	SlashInfo.Amount = 15;
	SlashInfo.DamageType = EM_DamageType::Melee;


	if (NotifyName == " AOESlash")
		AttackComponent->AOEDamage(300, AOESlashInfo);
	if (NotifyName == "Slash")
		AttackComponent->SphereTraceDamage(220, 40, SlashInfo);
}

