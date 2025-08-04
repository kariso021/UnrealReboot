// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/EnemyBase.h"
#include "AI/AIC_EnemyBaseCPP.h"
#include "Components/CapsuleComponent.h"
#include "../Widget/W_HealthBarCPP.h"

// Sets default values
AEnemyBase::AEnemyBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(35.f, 90.0f);
	//Create an HealthBar Component and Attatch to Mesh component
	HealthBarComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
	HealthBarComp->SetupAttachment(GetMesh()); // Mesh�� �ٷ� ����
	//HealthBarComp->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f)); // �Ӹ� ���� ��ġ ����
	//HealthBarComp->SetWidgetSpace(EWidgetSpace::Screen); // ȭ�� ������ ǥ��
	//HealthBarComp->SetDrawSize(FVector2D(200.0f, 50.0f)); // �ｺ�� ũ�� ����

	AttackComponent = CreateDefaultSubobject<UCP_Attacks>(TEXT("AttackComponent"));
	DamageSystemComponent = CreateDefaultSubobject<UDamageSystem>(TEXT("DamageSystemComponent"));
	DamageSystemComponent->SetMaxhealth(100);
	DamageSystemComponent->SetHealth(100);
	TeamNumber = 1;

}

void AEnemyBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}

UBehaviorTree* AEnemyBase::GetBehaviorTree()
{
	return BehaviorTree;
}


// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	


	/*AICComp = Cast<AAIC_EnemyBaseCPP>(GetController());*///�����ɷȴ�... BaseEnemyPawn���� Controller�� AICEnemyBase�� �����ؼ� ����ϸ� �ȴ�.
	if (DamageSystemComponent)
	{
		DamageSystemComponent->OnBlocked.AddDynamic(this, &AEnemyBase::HandleBlockedEvent);
		//Delegate �̿� Function Pointer �� ����� ���� �ƴѰ�?



		//--------------------------Helath Bar-------------------------------------------
		if (HealthBarComp && HealthBarComp->GetUserWidgetObject() == nullptr)
		{
			if (HealthBarComp->GetUserWidgetObject() == nullptr)
			{
				UW_HealthBarCPP* HealthBarWidget = CreateWidget<UW_HealthBarCPP>(GetWorld(), HealthBarWidgetClass);
				if (HealthBarWidget)
				{
					HealthBarComp->SetWidget(HealthBarWidget);
					HealthBarWidget->SetDamageableActor(this);
				}
			}

			HealthBarComp->SetVisibility(true);
		}

		HealthBarComp->SetVisibility(true);//Propagate To Children  �ڵ�ȭ(Protect �� Comp ��������)
	

		//---------------------------------------------------------------------------------------------------------
			DamageSystemComponent->OnDeath.AddDynamic(this, &AEnemyBase::Die);
			DamageSystemComponent->OnDamageResponse.AddDynamic(this, &AEnemyBase::HitResponse);
	}
	
}

// Called every frame
void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float AEnemyBase::GetCurrentHealth_Implementation()
{
	return DamageSystemComponent->GetCurrentHP_FromComponent();
}

float AEnemyBase::GetMaxHealth_Implementation()
{
	return DamageSystemComponent->GetMaxHP_FromComponent();
}

float AEnemyBase::Heal_Implementation(float Amount)
{
	return DamageSystemComponent->HealCPP(Amount);
}

bool AEnemyBase::IsDead_Implementation()
{
	return DamageSystemComponent->GetIsDead();
}

bool AEnemyBase::TakeDamage_Implementation(FDamageInfo& DamageInfo, AActor* DamageCauser)
{
	if (DamageInfo.CanBeBlocked)
	{	
		TryToBlock();	
	}
	return DamageSystemComponent->TakeDamageCPP(DamageInfo, DamageCauser);
}

bool AEnemyBase::IsAttacking_Implementation()
{
	return Attacking;
}

bool AEnemyBase::ReserveAttackToken_Implementation(int Amount)
{
	return DamageSystemComponent->ReserveAttackTokenCPP(Amount);
}

void AEnemyBase::ReturnAttackToken_Implementation(int Amount)
{
	DamageSystemComponent->ReturnAttackTokenCPP(Amount);
}

int AEnemyBase::GetTeamNumber_Implementation()
{
	return TeamNumber;
}

void AEnemyBase::SetIsInterruptible_Implementation(bool Value)
{
	DamageSystemComponent->SetIsInterruptible_FromComponent(Value);
}

void AEnemyBase::SetIsInvincible_Implementation(bool value)
{
	DamageSystemComponent->SetInvincible(value);
}

float AEnemyBase::SetMovementSpeed_Implementation(EM_MovementSpeed Speed)
{
	
	switch(Speed){
	case EM_MovementSpeed::Idle:
		GetCharacterMovement()->MaxWalkSpeed = 0;
		return 0;
		break;
	case EM_MovementSpeed::Walk:
		GetCharacterMovement()->MaxWalkSpeed = 100;
		return 100.0f;
		break;
	case EM_MovementSpeed::Jogging:
		GetCharacterMovement()->MaxWalkSpeed = 300;
		return 300.0f;
		break;
	case EM_MovementSpeed::Sprinting:
		GetCharacterMovement()->MaxWalkSpeed = 500;
		return 500.0f;
		break;
	default:
		return 0;
		break;	
	}
}

//FRangeofState AEnemyBase::GetIdealRange_Implementation()
//{
//	FRangeofState ReturnValue;
//	ReturnValue.AttackRadius = 150.0f;
//	ReturnValue.DefendRadius = 300.0f;
//	return ReturnValue;
//}

float AEnemyBase::GetAttackRadius_Implementation()
{
	return 150.0f;
}

float AEnemyBase::GetDefendRadius_Implementation()
{
	return 300.0f;
}


void AEnemyBase::EquipWeapon_Implementation()
{
}

void AEnemyBase::UnequipWeapon_Implementation()
{
}

void AEnemyBase::Attack_Implementation(AActor* AttackTarget)
{
	Attacking = true;
}

void AEnemyBase::JumpToDestination_Implementation(FVector Destination)
{
	FVector StartPos = GetActorLocation();
	FVector LaunchVelocity;

	// ������ ����Ͽ� ���� �ӵ��� ���ϱ�
	bool bHaveAimSolution = UGameplayStatics::SuggestProjectileVelocity_CustomArc(
		this,                       // WorldContextObject
		LaunchVelocity,             // OutLaunchVelocity, ���� �߻� �ӵ��� ����� ����
		StartPos,                   // �߻� ��ġ
		Destination,                // ��ǥ ��ġ
		0.0f,                       // OverrideGravityZ, �߷°� ������ (�⺻�� ��� �� 0)
		0.5f                        // ArcParam, ������ ȣ�� �����ϴ� �Ķ���� (�⺻�� 0.5)
	);

	// ���� ������ ���� ĳ���͸� �߻�
	if (bHaveAimSolution)
	{
		ACharacter* Character = Cast<ACharacter>(this);
		if (Character)
		{
			Character->LaunchCharacter(LaunchVelocity, true, true);
		}
	}
}

bool AEnemyBase::AttackStart_Implementation(AActor* AttackTarget, int TokenNeeded)
{
	if (AttackTarget && AttackTarget->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()))
	{
		if (IDamageableInterface::Execute_ReserveAttackToken(AttackTarget, TokenNeeded))
		{
			IEnemyAIInterface::Execute_StoreAttackToken(this, AttackTarget, TokensUsedInCurrentAttack * -1);
			Attacking = false;
			TokensUsedInCurrentAttack = TokenNeeded;
			return true;
		}
		else
		{
			return false;
		}
	}

	return false;
}

void AEnemyBase::AttackEnd_Implementation(AActor* AttackTarget)
{

	//Test �뵵�� ����ٰ� �صа�
	//UE_LOG(LogTemp, Display, TEXT("AttackEnd BroadCast"));
	OnAttackEnd.Broadcast();
	if (AttackTarget && AttackTarget->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()))
	{
		IDamageableInterface::Execute_ReturnAttackToken(AttackTarget, TokensUsedInCurrentAttack);
		IEnemyAIInterface::Execute_StoreAttackToken(this,AttackTarget, TokensUsedInCurrentAttack*-1);
		Attacking = false;
	}
}

void AEnemyBase::StoreAttackToken_Implementation(AActor* AttackTarget, int Amount)
{
	if (AttackTarget == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Attack target is null."));
		return;
	}

	if (AttackTokens.Contains(AttackTarget))
	{
		// �ش� Ű�� �̹� ����� ������ Amount����
		AttackTokens[AttackTarget] += Amount;
	}
	else
	{
		// ���ο� Ű�� �ʿ� �߰�
		AttackTokens.Add(AttackTarget, Amount);
	}
	UE_LOG(LogTemp, Log, TEXT("Stored %d tokens for %s."), Amount, *AttackTarget->GetName());
}

APatrolRoute* AEnemyBase::GetPatrolRoute_Implementation()
{
	return PatrolRoute;
}

void AEnemyBase::Die()
{
	AAIC_EnemyBaseCPP* AICComp = Cast<AAIC_EnemyBaseCPP>(GetController());//��¥ ������ �ȵ�� ����ε� ��¿���� ����
	WhenDied=true;
	StopAnimMontage();
	
	GetMesh()->SetSimulatePhysics(true);//Mesh�� Character.h �� private ������ �����Ǿ��־ �̷��� �޾ƿ;���
	//GetMesh()->SetCollisionEnabled()//�� ��� ��������
	//AICComp->BrainComponent->StopLogic(false);
	AICComp->SetStateAsDead();




}


void AEnemyBase::StartBlock()
{
	GetWorldTimerManager().ClearTimer(HoldBlockTimer);
	HoldBlockTimer.Invalidate();

	GetCharacterMovement()->StopMovementImmediately();
	DamageSystemComponent->SetIsBlocking(true);
	GetCharacterMovement()->StopMovementImmediately();

	GetWorldTimerManager().SetTimer(HoldBlockTimer, this, &AEnemyBase::EndBlock, 2.0f, false);
}

void AEnemyBase::HitResponse(EM_DamageResponse damageresponse, AActor* DamageCauser)
{


}

void AEnemyBase::EndBlock()
{
	DamageSystemComponent->SetIsBlocking(false);
	OnBlockEnded.Broadcast();
}

void AEnemyBase::HandleBlockedEvent(bool bCanBeParried, AActor* DamageCauser)
{
	GetWorldTimerManager().ClearTimer(HoldBlockTimer);

	HoldBlockTimer.Invalidate();

}

void AEnemyBase::TryToBlock()
{
	//randomfloat�� �ʿ���
	float RandomValue = FMath::FRandRange(0.0f, 1.0f);
	if (RandomValue < BlockChance)//0.5 ������ ����
	{
		StartBlock();
	}
}

bool AEnemyBase::GetisWieldingWeapon()
{
	return IsWieldingWeapon;
}

