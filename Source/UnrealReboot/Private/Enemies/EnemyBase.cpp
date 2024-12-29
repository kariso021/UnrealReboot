// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/EnemyBase.h"
#include "AIC_EnemyBaseCPP.h"

// Sets default values
AEnemyBase::AEnemyBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;



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



		//--------------------------Helath Bar ��� ��Ȱ��ȭ-------------------------------------------
		if (HealthBarComp && HealthBarComp->GetUserWidgetObject() == nullptr)
		{
			if (HealthBarComp->GetUserWidgetObject() == nullptr)
			{
				UUserWidget* HealthBarWidget = CreateWidget<UUserWidget>(GetWorld(), HealthBarWidgetClass);
				if (HealthBarWidget)
				{
					HealthBarComp->SetWidget(HealthBarWidget);
				}
			}

			HealthBarComp->SetVisibility(true);
		}
		HealthBarComp->SetVisibility(true);//Propagate To Children  �ڵ�ȭ(Protect �� Comp ��������)
	
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

float AEnemyBase::GetCurrentHealth()
{
	return DamageSystemComponent->GetCurrentHP_FromComponent();
}

float AEnemyBase::GetMaxHealth()
{
	return DamageSystemComponent->GetMaxHP_FromComponent();
}

float AEnemyBase::Heal(float Amount)
{
	return DamageSystemComponent->HealCPP(Amount);
}

bool AEnemyBase::IsDead()
{
	return DamageSystemComponent->GetIsDead();
}

bool AEnemyBase::TakeDamage(FDamageInfo& DamageInfo, AActor* DamageCauser)
{
	return DamageSystemComponent->TakeDamageCPP(DamageInfo, DamageCauser);
}

bool AEnemyBase::IsAttacking()
{
	return Attacking;
}

bool AEnemyBase::ReserveAttackToken(int Amount)
{
	return DamageSystemComponent->ReserveAttackTokenCPP(Amount);
}

void AEnemyBase::ReturnAttackToken(int Amount)
{
	DamageSystemComponent->ReturnAttackTokenCPP(Amount);
}

int AEnemyBase::GetTeamNumber()
{
	return TeamNumber;
}

void AEnemyBase::SetIsInterruptible(bool Value)
{
	DamageSystemComponent->SetIsInterruptible_FromComponent(Value);
}

void AEnemyBase::SetIsInvincible(bool value)
{
	DamageSystemComponent->SetInvincible(value);
}

float AEnemyBase::SetMovementSpeed(EM_MovementSpeed Speed)
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

FRangeofState AEnemyBase::GetIdealRange()
{
	FRangeofState ReturnValue;
	ReturnValue.AttackRadius = 150.0f;
	ReturnValue.DefendRadius = 300.0f;
	return ReturnValue;
}

void AEnemyBase::EquipWeapon()
{

}

void AEnemyBase::UnequipWeapon()
{

}

void AEnemyBase::Attack(AActor* AttackTarget)
{
	Attacking = true;
}

void AEnemyBase::JumpToDestination(FVector Destination)
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

bool AEnemyBase::AttackStart(AActor* AttackTarget, int TokenNeeded)
{
	//IDamageableInterface* owner_dmg_interface = Cast<IDamageableInterface>(GetOwner());
	IDamageableInterface* AttackTargetdInterface = Cast<IDamageableInterface>(AttackTarget);

	if (AttackTargetdInterface)
	{
		if (AttackTargetdInterface->ReserveAttackToken(TokenNeeded))
		{
			StoreAttackToken(AttackTarget, TokensUsedInCurrentAttack);
			Attacking = false;
			TokensUsedInCurrentAttack = TokenNeeded;
			return true;
		}
		else {
			return false;
		}
	}



	return false;
}

void AEnemyBase::AttackEnd(AActor* AttackTarget)
{
	//IDamageableInterface* owner_dmg_interface = Cast<IDamageableInterface>(GetOwner());
	IDamageableInterface* AttackTargetdInterface = Cast<IDamageableInterface>(AttackTarget);

	if (AttackTargetdInterface)
	{
		AttackTargetdInterface->ReturnAttackToken(TokensUsedInCurrentAttack);
		StoreAttackToken(AttackTarget, -1 * TokensUsedInCurrentAttack);
		Attacking = false;
		OnAttackEnd.Broadcast();
	}

}

void AEnemyBase::StoreAttackToken(AActor* AttackTarget, int Amount)
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

APatrolRoute* AEnemyBase::GetPatrolRoute()
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

