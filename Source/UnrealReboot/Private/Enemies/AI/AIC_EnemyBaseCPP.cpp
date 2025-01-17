// Fill out your copyright notice in the Description page of Project Settings.

//#AIC_EnemyBaseCPP.cpp
#include "Enemies/AI/AIC_EnemyBaseCPP.h"
#include "DamageSystem/DamageableInterface.h"
#include "Enemies/EnemyBase.h"


AAIC_EnemyBaseCPP::AAIC_EnemyBaseCPP()//기본생성자
{
	//Perception Component 생성
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));

	// 시각 감지 설정
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	InitializeSightConfig();

	// 청각 감지 설정
	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
	InitializeHearingConfig();

	 //피해 감지는 설정 안해도 될듯

	// 이벤트 핸들러 연결
	AIPerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &AAIC_EnemyBaseCPP::OnPerceptionUpdated);

}



//--------------------------------------------Posses and Unposses

void AAIC_EnemyBaseCPP::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AEnemyBase* EnemyBase = Cast<AEnemyBase>(InPawn);
	if (EnemyBase && EnemyBase->BehaviorTree)
	{
		// Behavior Tree 실행
		if (!RunBehaviorTree(EnemyBase->BehaviorTree))
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to run Behavior Tree!"));
			return;
		}

		// Blackboard 초기화
		BlackboardComp = GetBlackboardComponent();
		if (!BlackboardComp)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to initialize Blackboard Component!"));
			return;
		}

		// InPawn이 EnemyAIInterface를 구현했는지 확인
		if (InPawn && InPawn->GetClass()->ImplementsInterface(UEnemyAIInterface::StaticClass()))
		{
			// 인터페이스의 Execute_ 함수를 호출하여 데이터를 가져옴
		/*	FRangeofState IdealRange = IEnemyAIInterface::Execute_GetIdealRange(InPawn);*/

			float AttackRadius = IEnemyAIInterface::Execute_GetAttackRadius(InPawn);
			float DefendRadius = IEnemyAIInterface::Execute_GetDefendRadius(InPawn);

			// Blackboard에 데이터 설정
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

		// 초기 상태 설정
		SetStateAsPassive();

		// 주기적으로 CheckForgottenSeenActor 호출
		GetWorldTimerManager().SetTimer(CheckForgottenActorsTimer, this, &AAIC_EnemyBaseCPP::CheckForgottenSeenActor, 0.5f, true);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No Valid Behavior Tree in Enemy Actor"));
	}
}

void AAIC_EnemyBaseCPP::OnUnPossess()
{
	Super::OnUnPossess();  // 상위 클래스의 OnUnPossess 호출

	GetWorldTimerManager().ClearTimer(CheckForgottenActorsTimer);
	CheckForgottenActorsTimer.Invalidate();
}

void AAIC_EnemyBaseCPP::BeginPlay()
{
	Super::BeginPlay();

	AIPerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &AAIC_EnemyBaseCPP::OnPerceptionUpdated);

}

void AAIC_EnemyBaseCPP::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)//다수의 Actor로부터 감지
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
	SightConfig->SightRadius = 1500.0f;
	SightConfig->LoseSightRadius = 1600.0f;
	SightConfig->PeripheralVisionAngleDegrees = 90.0f;
	AIPerceptionComponent->ConfigureSense(*SightConfig);

}

void AAIC_EnemyBaseCPP::InitializeHearingConfig()
{
	HearingConfig->HearingRange = 2500.0f;
	HearingConfig->bUseLoSHearing = true;  // Line of Sight 청각을 사용할지 설정
	HearingConfig->LoSHearingRange = 1800.0f;
	AIPerceptionComponent->ConfigureSense(*HearingConfig);

}

bool AAIC_EnemyBaseCPP::MatchSenseType(const FAISenseID& StimulusType, EM_AISense SenseType) const//해당 함수 멤버변수 안바꿔야겠지
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
	BlackboardComp->SetValueAsEnum(StateKeyName, static_cast<uint8>(EM_AIState::Passive));//cast를 한번 해줘야되네
}

void AAIC_EnemyBaseCPP::SetStateAsAttacking(AActor* attacktarget, bool UseLastKnownAttackTarget)
{
	AActor* NewAttackTarget = nullptr;

	// AttackTarget이 유효하고 UseLastKnownAttackTarget이 설정된 경우 이를 사용, 그렇지 않으면 attacktarget을 사용
	if (IsValid(AttackTarget) && UseLastKnownAttackTarget)
	{
		NewAttackTarget = AttackTarget;
	}
	else
	{
		NewAttackTarget = attacktarget;
	}

	// NewAttackTarget이 유효한지 확인
	if (IsValid(NewAttackTarget))
	{
		// NewAttackTarget이 DamageableInterface를 구현하는지 확인
		if (NewAttackTarget->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()))
		{
			// IsDead 메시지를 호출
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
			// 인터페이스를 구현하지 않는 경우 상태를 Passive로 설정
			SetStateAsPassive();
		}
	}
	else
	{
		// NewAttackTarget이 유효하지 않은 경우 상태를 Passive로 설정
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
				SeekAttackTargetTimer.Invalidate();//Invalidate 오락가락하네
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


			//일정 기간 탐색하다가 TimeToSeekAfterLosingSight 동안만 발동하고싶음
			GetWorldTimerManager().SetTimer(SeekAttackTargetTimer, this, &AAIC_EnemyBaseCPP::SeekAttackTarget, TimeToSeekAfterLosingSight, false);
		}
		break; //Case문안에서 선언으르 원할시 중괄호로 묶어둬야함 -> case 문 내에서 변수를 선언하면 컴파일 시 스택영역의 크기를 정확히 알 수 없기 때문
		
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

bool AAIC_EnemyBaseCPP::OnSameTeam(AActor* OtherActor)//어렵다... Message 함수 Damagealbe Interface 를 어떻게 여기에다가 받아오지?
{
	if (!OtherActor || !GetPawn())
	{
		return false;
	}

	// IBP_Damageable 인터페이스를 구현하는지 확인
	if (!OtherActor->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()) ||
		!GetPawn()->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()))
	{
		return false;
	}

	// 팀 번호 가져오기
	int32 MyTeamNumber = IDamageableInterface::Execute_GetTeamNumber(GetPawn());
	int32 OtherTeamNumber = IDamageableInterface::Execute_GetTeamNumber(OtherActor);

	// 팀 번호 비교
	return MyTeamNumber == OtherTeamNumber;
}


FCheckSensedStimulus AAIC_EnemyBaseCPP::CanSenseActor(AActor* Actor, EM_AISense SenseType)
{
	FCheckSensedStimulus Result;

	// 인식 정보 구조체
	FActorPerceptionBlueprintInfo Info;

	// 주어진 액터에 대한 인식 정보를 얻음
	if (AIPerceptionComponent && AIPerceptionComponent->GetActorsPerception(Actor, Info))
	{
		// 모든 감지된 자극들을 순회
		for (FAIStimulus& Stimulus : Info.LastSensedStimuli)
		{
			if (Stimulus.IsValid() && Stimulus.WasSuccessfullySensed())
			{
				// 자극의 유형 확인
				if (MatchSenseType(Stimulus.Type, SenseType))
				{
					Result.Stimulus = Stimulus;
					Result.Sensed = true;
					break; // 매칭되는 자극을 찾았으므로 루프 탈출
				}
			}
		}
	}

	return Result; // 결과 반환
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

		// 잊혀진 액터들을 확인
		for (AActor* KnownActor : KnownSeenActors)
		{
			if (!currentPercieved.Contains(KnownActor))//Find 함수의 내용과 비슷함
			{
				HandleForgottenActor(KnownActor);
			}
		}
		//업데이트 부분 필요없어  KnownSeenActors = currentPercieved;->Handle에서 Remove로 지울거야
	}
}

//잊혀져버린 Actor를 다루는 부분-> 시야사라졌을때
void AAIC_EnemyBaseCPP::HandleForgottenActor(AActor* Actor)
{
	if (!Actor) {
		return; // Actor가 유효하지 않으면 함수를 종료
	}

	// 액터를 KnownSeenActors 배열에서 제거
	KnownSeenActors.Remove(Actor);

	// 액터가 현재의 공격 대상인지 확인
	if (Actor == AttackTarget)
	{
		// 공격 대상이면 상태를 Passive로 변경
		SetStateAsPassive();

		// 공격 대상을 null로 초기화-> 초기화 안해도 돌아가지만 가끔 오류가 뜸
		AttackTarget = nullptr;
	}
}






