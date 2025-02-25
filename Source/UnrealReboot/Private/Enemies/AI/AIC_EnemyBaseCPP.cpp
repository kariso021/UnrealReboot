// Fill out your copyright notice in the Description page of Project Settings.

//#AIC_EnemyBaseCPP.cpp
#include "Enemies/AI/AIC_EnemyBaseCPP.h"
#include "DamageSystem/DamageableInterface.h"
#include "Enemies/EnemyBase.h"


AAIC_EnemyBaseCPP::AAIC_EnemyBaseCPP()//�⺻������
{
	//Perception Component ����
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));

	// �ð� ���� ����
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	InitializeSightConfig();

	// û�� ���� ����
	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
	InitializeHearingConfig();

	DamageSenseConfig= CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("DamageSenseConfig"));
	InitializeDamageSenseConfig();

	AIPerceptionComponent->SetDominantSense(UAISense_Sight::StaticClass());



	// Blackboard Key �ʱ�ȭ
	AttackTargetKeyName = "AttackTarget";
	StateKeyName = "State";                              // AI�� ����
	PointOfInterestKeyName = "PointOfInterest"; 
	AttackRadiusKeyName = "AttackRadius";                // ���� �ݰ�
	DefendRadiusKeyName = "DefendRadius";                // ��� �ݰ�
	DistanceToAttackTargetKeyName = "DistanceToAttackTarget"; // ���� ������ �Ÿ�


	 //���� ������ ���� ���ص� �ɵ�

	// �̺�Ʈ �ڵ鷯 ����
	AIPerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &AAIC_EnemyBaseCPP::OnPerceptionUpdated);

}



//--------------------------------------------Posses and Unposses

void AAIC_EnemyBaseCPP::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	AEnemyBase* EnemyBase = Cast<AEnemyBase>(InPawn);
	if (UBehaviorTree* const Tree = EnemyBase->GetBehaviorTree())
	{
		// Blackboard ��� �� �ʱ�ȭ
		if (!UseBlackboard(Tree->BlackboardAsset, BlackboardComp))
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to initialize Blackboard with the provided asset!"));
			return;
		}

		// UseBlackboard ���Ŀ� BlackboardComp ��������
		BlackboardComp = GetBlackboardComponent();
		if (!BlackboardComp)
		{
			UE_LOG(LogTemp, Error, TEXT("Blackboard Component is null after UseBlackboard!"));
			return;
		}

		Blackboard = BlackboardComp;

		// BehaviorTree ����
		if (!RunBehaviorTree(Tree))
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to run BehaviorTree!"));
			return;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Enemy Actor does not have a valid Behavior Tree!"));
	}


		if (InPawn && InPawn->GetClass()->ImplementsInterface(UEnemyAIInterface::StaticClass())) //InPawn->Implements<UEnemyAIInterface()> �� �� ����Ǵ� ���
		{
			// �������̽��� Execute_ �Լ��� ȣ���Ͽ� �����͸� ������
		/*	FRangeofState IdealRange = IEnemyAIInterface::Execute_GetIdealRange(InPawn);*/

			float AttackRadius = IEnemyAIInterface::Execute_GetAttackRadius(InPawn);
			float DefendRadius = IEnemyAIInterface::Execute_GetDefendRadius(InPawn);

			// Blackboard�� ������ ����
			if (BlackboardComp)
			{
				BlackboardComp->SetValueAsFloat(AttackRadiusKeyName, AttackRadius);
				BlackboardComp->SetValueAsFloat(DefendRadiusKeyName, DefendRadius);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Blackboard Component is null!"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("InPawn does not implement IEnemyAIInterface!"));
		}

		// �ʱ� ���� ����
		SetStateAsPassive();

		// �ֱ������� CheckForgottenSeenActor ȣ��
		GetWorldTimerManager().SetTimer(CheckForgottenActorsTimer, this, &AAIC_EnemyBaseCPP::CheckForgottenSeenActor, 0.5f, true);

}

void AAIC_EnemyBaseCPP::OnUnPossess()
{
	Super::OnUnPossess();  

	GetWorldTimerManager().ClearTimer(CheckForgottenActorsTimer);
	CheckForgottenActorsTimer.Invalidate();
}

void AAIC_EnemyBaseCPP::BeginPlay()
{
	Super::BeginPlay();

	if (AIPerceptionComponent)
	{
		AIPerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &AAIC_EnemyBaseCPP::OnPerceptionUpdated);//�̹� �𸮾󿡼� ���ε��Ȱ�
	}

}

void AAIC_EnemyBaseCPP::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)//�ټ��� Actor�κ��� ����
{
	for (AActor* Actor : UpdatedActors)
	{
		if (CanSenseActor(Actor, EM_AISense::Sight).Sensed)
			HandleSensedSight(Actor);
		else
			HandleLostSight(Actor);
		if (CanSenseActor(Actor, EM_AISense::Hearing).Sensed)
			HandleSensedSound(CanSenseActor(Actor, EM_AISense::Hearing).Stimulus.StimulusLocation);
		if (CanSenseActor(Actor, EM_AISense::Damage).Sensed)
			HandleSensedDamage(Actor);

	}
}
//-----------------------------------ConfigOfSense
void AAIC_EnemyBaseCPP::InitializeSightConfig()
{
	SightConfig->SightRadius = 1500.0f; // �ð� ���� �ݰ�
	SightConfig->LoseSightRadius = 2000.0f; // �þ߸� �Ҵ� �ݰ�

	// �þ߰� ���� (ȭ���� �ݰ�)
	SightConfig->PeripheralVisionAngleDegrees = 60.0f; // �� �þ߰��� 120��

	// ���� �Ҽ� ���� (�� ���� ��)
	SightConfig->DetectionByAffiliation.bDetectEnemies = true; // �� ����
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true; // �Ʊ� ����
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true; // �߸� ����


	// ������ �ִ� ��ȿ �ð�
	SightConfig->SetMaxAge(20.0f); // ���� ������ 20�� ���� ��ȿ

	AIPerceptionComponent->ConfigureSense(*SightConfig);

}

void AAIC_EnemyBaseCPP::InitializeHearingConfig()
{
	HearingConfig->HearingRange = 500.0f;
	HearingConfig->bUseLoSHearing = false;  // Line of Sight û���� ������� ����


	HearingConfig->DetectionByAffiliation.bDetectEnemies = true; // �� ����
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = true; // �Ʊ� ���� �� ��
	HearingConfig->DetectionByAffiliation.bDetectNeutrals = true; // �߸� ���� �� ��
	HearingConfig->SetMaxAge(3.0f);


	AIPerceptionComponent->ConfigureSense(*HearingConfig);

}

void AAIC_EnemyBaseCPP::InitializeDamageSenseConfig()
{
	DamageSenseConfig->SetMaxAge(5.0f);
	AIPerceptionComponent->ConfigureSense(*DamageSenseConfig);

	// ����� ���� ����
}

bool AAIC_EnemyBaseCPP::MatchSenseType(const FAISenseID& StimulusType, EM_AISense SenseType) const//�ش� �Լ� ������� �ȹٲ�߰���
{
	static const FAISenseID SightID = UAISense_Sight::StaticClass()->GetDefaultObject<UAISense_Sight>()->GetSenseID();
	static const FAISenseID HearingID = UAISense_Hearing::StaticClass()->GetDefaultObject<UAISense_Hearing>()->GetSenseID();
	static const FAISenseID DamageID = UAISense_Damage::StaticClass()->GetDefaultObject<UAISense_Damage>()->GetSenseID();

	switch (SenseType)
	{
	case EM_AISense::Sight:
		return StimulusType == SightID;
		break;
	case EM_AISense::Hearing:
		return StimulusType == HearingID;
		break;
	case EM_AISense::Damage:
		return StimulusType == DamageID;
		break;
	default:
		return false;
		break;
	}
}



//-------------------------------------------------------------------------------------------------




void AAIC_EnemyBaseCPP::SetStateAsPassive()
{
	BlackboardComp->SetValueAsEnum(StateKeyName, static_cast<uint8>(EM_AIState::Passive));//cast�� �ѹ� ����ߵǳ�
}

void AAIC_EnemyBaseCPP::SetStateAsAttacking(AActor* attacktarget, bool UseLastKnownAttackTarget)
{
	AActor* NewAttackTarget = nullptr;

	// AttackTarget�� ��ȿ�ϰ� UseLastKnownAttackTarget�� ������ ��� �̸� ���, �׷��� ������ attacktarget�� ���
	if (IsValid(AttackTarget) && UseLastKnownAttackTarget)
	{
		NewAttackTarget = AttackTarget;
	}
	else
	{
		NewAttackTarget = attacktarget;
	}

	// NewAttackTarget�� ��ȿ���� Ȯ��
	if (IsValid(NewAttackTarget))
	{
		// NewAttackTarget�� DamageableInterface�� �����ϴ��� Ȯ��
		if (NewAttackTarget->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()))
		{
			// IsDead �޽����� ȣ��
			if (IDamageableInterface::Execute_IsDead(NewAttackTarget))
			{
				SetStateAsPassive();
			}
			else
			{
				BlackboardComp->SetValueAsObject(AttackTargetKeyName, attacktarget);
				BlackboardComp->SetValueAsEnum(StateKeyName, static_cast<uint8>(EM_AIState::Attacking));
				AttackTarget = NewAttackTarget;
			}
		}
		else
		{
			// �������̽��� �������� �ʴ� ��� ���¸� Passive�� ����
			SetStateAsPassive();
		}
	}
	else
	{
		// NewAttackTarget�� ��ȿ���� ���� ��� ���¸� Passive�� ����
		SetStateAsPassive();
	}

}

void AAIC_EnemyBaseCPP::HandleSensedSight(AActor* otherActor)
{
	KnownSeenActors.AddUnique(AttackTarget);
	if (!OnSameTeam(otherActor))
	{
		switch (GetCurrentState())
		{
		case EM_AIState::Passive:
		case EM_AIState::Investigating:
		case EM_AIState::Seeking:
			SetStateAsAttacking(otherActor, false);
			break;
		case EM_AIState::Attacking:
			if (otherActor == AttackTarget) {
				GetWorldTimerManager().ClearTimer(SeekAttackTargetTimer);
				SeekAttackTargetTimer.Invalidate();//Invalidate ���������ϳ�
			}
			break;
		default:
			break;
		}
		
	}
	
}

void AAIC_EnemyBaseCPP::HandleSensedSound(FVector Location)
{
	switch (GetCurrentState())
	{
	case EM_AIState::Passive:
	case EM_AIState::Investigating:
	case EM_AIState::Seeking:
		SetStateAsInvestigating(Location);
		break;
	default:
		break;
	}
}

void AAIC_EnemyBaseCPP::SetStateAsInvestigating(FVector Location)
{
	BlackboardComp->SetValueAsEnum(StateKeyName, static_cast<uint8>(EM_AIState::Investigating));
	BlackboardComp->SetValueAsVector(PointOfInterestKeyName, Location);
}

EM_AIState AAIC_EnemyBaseCPP::GetCurrentState()
{
	return static_cast<EM_AIState>(BlackboardComp->GetValueAsEnum(StateKeyName));
}

void AAIC_EnemyBaseCPP::HandleSensedDamage(AActor* actor)
{
	if (!OnSameTeam(actor))
	{
		switch (GetCurrentState())
		{
		case EM_AIState::Passive:
		case EM_AIState::Investigating:
		case EM_AIState::Seeking:
			SetStateAsAttacking(actor, false);
			break;
		default:
			break;
		}
	}
}

void AAIC_EnemyBaseCPP::SetStateAsDead()
{
	BlackboardComp->SetValueAsEnum(StateKeyName, static_cast<uint8>(EM_AIState::Dead));
}

void AAIC_EnemyBaseCPP::SetStateAsFrozen()
{
	BlackboardComp->SetValueAsEnum(StateKeyName, static_cast<uint8>(EM_AIState::Frozen));
}

void AAIC_EnemyBaseCPP::HandleForgotActor(AActor* Actor)
{
	KnownSeenActors.Remove(Actor);
	if (Actor == AttackTarget)
	{
		SetStateAsPassive();
	}
}

void AAIC_EnemyBaseCPP::HandleLostSight(AActor* Actor)
{
	if (AttackTarget == Actor)
	{
		
		switch (GetCurrentState())
		{
		case EM_AIState::Attacking:
		case EM_AIState::Frozen:
		case EM_AIState::Investigating:
		{
			GetWorldTimerManager().ClearTimer(SeekAttackTargetTimer);
			SeekAttackTargetTimer.Invalidate();


			//���� �Ⱓ Ž���ϴٰ� TimeToSeekAfterLosingSight ���ȸ� �ߵ��ϰ����
			GetWorldTimerManager().SetTimer(SeekAttackTargetTimer, this, &AAIC_EnemyBaseCPP::SeekAttackTarget, TimeToSeekAfterLosingSight, false);
		}
		break; 
		
		default:
			break;
		}
	}
	
}

void AAIC_EnemyBaseCPP::SetStateAsSeeking(FVector Location)
{
	BlackboardComp->SetValueAsEnum(StateKeyName, static_cast<uint8>(EM_AIState::Seeking));
	BlackboardComp->SetValueAsVector(PointOfInterestKeyName, Location);
}

bool AAIC_EnemyBaseCPP::OnSameTeam(AActor* OtherActor)//��ƴ�... Message �Լ� Damagealbe Interface �� ��� ���⿡�ٰ� �޾ƿ���?
{
	if (!OtherActor || !GetPawn())
	{
		return false;
	}

	// IBP_Damageable �������̽��� �����ϴ��� Ȯ��
	if (!OtherActor->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()) ||
		!GetPawn()->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()))
	{
		return false;
	}

	// �� ��ȣ ��������
	int32 MyTeamNumber = IDamageableInterface::Execute_GetTeamNumber(GetPawn());
	int32 OtherTeamNumber = IDamageableInterface::Execute_GetTeamNumber(OtherActor);

	// �� ��ȣ ��
	return MyTeamNumber == OtherTeamNumber;
}


FCheckSensedStimulus AAIC_EnemyBaseCPP::CanSenseActor(AActor* Actor, EM_AISense SenseType)
{
	FCheckSensedStimulus Result;

	// �ν� ���� ����ü
	FActorPerceptionBlueprintInfo Info;

	// �־��� ���Ϳ� ���� �ν� ������ ����
	if (AIPerceptionComponent && AIPerceptionComponent->GetActorsPerception(Actor, Info))
	{
		// ��� ������ �ڱص��� ��ȸ
		for (FAIStimulus& Stimulus : Info.LastSensedStimuli)
		{
			if (Stimulus.IsValid() && Stimulus.WasSuccessfullySensed())
			{
				// �ڱ��� ���� Ȯ��
				if (MatchSenseType(Stimulus.Type, SenseType))
				{
					Result.Stimulus = Stimulus;
					Result.Sensed = true;
					break; 
				}
			}
		}
	}

	return Result; // ��� ��ȯ
}

void AAIC_EnemyBaseCPP::SeekAttackTarget()
{
	SetStateAsSeeking(AttackTarget->GetActorLocation());
	GetWorldTimerManager().ClearTimer(SeekAttackTargetTimer);
	SeekAttackTargetTimer.Invalidate();
}

void AAIC_EnemyBaseCPP::CheckForgottenSeenActor()
{
	TArray<AActor*> CurrentlyPerceivedActors;
	AIPerceptionComponent->GetKnownPerceivedActors(UAISense_Sight::StaticClass(), CurrentlyPerceivedActors);

	int32 NumCurrentlyPerceivedActors = CurrentlyPerceivedActors.Num();
	int32 NumKnownSeenActors = KnownSeenActors.Num();


	if (NumCurrentlyPerceivedActors != NumKnownSeenActors)
	{
		TArray<AActor*> currentPercieved;
		AIPerceptionComponent->GetKnownPerceivedActors(UAISense_Sight::StaticClass(), currentPercieved);

		// ������ ���͵��� Ȯ��
		for (AActor* KnownActor : KnownSeenActors)
		{
			if (!currentPercieved.Contains(KnownActor))//Find �Լ��� ����� �����
			{
				HandleForgottenActor(KnownActor);
			}
		}
		//������Ʈ �κ� �ʿ����  KnownSeenActors = currentPercieved;->Handle���� Remove�� ����ž�
	}
}

//���������� Actor�� �ٷ�� �κ�-> �þ߻��������
void AAIC_EnemyBaseCPP::HandleForgottenActor(AActor* Actor)
{
	if (!Actor) {
		return;
	}

	// ���͸� KnownSeenActors �迭���� ����
	KnownSeenActors.Remove(Actor);

	// ���Ͱ� ������ ���� ������� Ȯ��
	if (Actor == AttackTarget)
	{
		// ���� ����̸� ���¸� Passive�� ����
		SetStateAsPassive();

		// ���� ����� null�� �ʱ�ȭ-> �ʱ�ȭ ���ص� ���ư����� ���� ������ ��
		AttackTarget = nullptr;
	}
}






