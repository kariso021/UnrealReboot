// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/MainPlayer.h"
#include "TimerManager.h"
#include <GameFramework/CharacterMovementComponent.h>
#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetMathLibrary.h>
#include "../Widget/Player_HUD_CPP.h"

// Sets default values
AMainPlayer::AMainPlayer()
{

    // Set size for collision capsule
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);


    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    //-----------------------------------------Character movement----------------------------------------
    // Configure character movement
    GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

    // Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
    // instead of recompiling to adjust them
    GetCharacterMovement()->JumpZVelocity = 700.f;
    GetCharacterMovement()->AirControl = 0.35f;
    GetCharacterMovement()->MaxWalkSpeed = 500.f;
    GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
    GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;




    // Create a camera boom (pulls in towards the player if there is a collision)
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
    CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

    // Create a follow camera
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
    FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm


    DamageSystemComponent = CreateDefaultSubobject<UDamageSystem>(TEXT("DamageSystemComponent"));

    AttacksComponent=CreateDefaultSubobject<UCP_Attacks>(TEXT("AttacksComponent)"));

    
    //------------Melee Attack 시작 전

    bMeleeAttackExcute = false;
    bRangeAttackExcute = false;

    //--------------------------------------- 에임 부분

     // 기본 및 조준 시 카메라 오프셋 설정
    DefaultBoomOffset = FVector(0.f, 60.f, 30.f);  // 기본 오프셋
    AimBoomOffset = FVector(200.f, 65.f, 45.f);      // 조준 시 카메라 당겨짐

    // Timeline 초기화
    AimTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("AimTimeline"));

    //-----------Dodge 변수

    readyToDodge = false;




}

// Called when the game starts or when spawned
void AMainPlayer::BeginPlay()
{
	Super::BeginPlay();

    if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        // Enhanced Input Subsystem 가져오기
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            // Input Mapping Context 추가
            Subsystem->AddMappingContext(InputMappingContext, 0);
        }
    }

    // Curve 데이터가 존재하면 Timeline 설정 -> 에임 부드럽게 해주는 부분
    if (AimCurve)
    {
        FOnTimelineFloat ProgressFunction;
        ProgressFunction.BindUFunction(this, FName("UpdateCameraOffset"));

        AimTimeline->AddInterpFloat(AimCurve, ProgressFunction);
    }

    DisplayHUD();
	
}

void AMainPlayer::OnConstruction(const FTransform& Transform)
{
    if (DamageSystemComponent)
    {
        DamageSystemComponent->SetAttackTokenCount(1);
    }
    if (GetMesh())
    {
        const FName BoneToHide = TEXT("Gun");
        GetMesh()->HideBoneByName(BoneToHide, EPhysBodyOp::PBO_None);
    }
}


void AMainPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMainPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {

        //Jumping
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

        //Moving
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMainPlayer::Move);

        //Looking
        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMainPlayer::Look);

        //ChangeStance
        EnhancedInputComponent->BindAction(Slot1Action, ETriggerEvent::Triggered, this, &AMainPlayer::Slot1);

        EnhancedInputComponent->BindAction(Slot2Action, ETriggerEvent::Triggered, this, &AMainPlayer::Slot2);

        EnhancedInputComponent->BindAction(Slot3Action, ETriggerEvent::Triggered, this, &AMainPlayer::Slot3);

        EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AMainPlayer::AcceptAttack);

        //Dodge

        EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Triggered, this, &AMainPlayer::DodgeTriggered);

        EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Completed, this, &AMainPlayer::DodgeCompleted);

    }


}


void AMainPlayer::ChangeStance(EPlayerStance NewStance)
{
    if (Stance == NewStance)
        return;
    EPlayerStance OldStance;
    OldStance = Stance; 
    Stance = NewStance; 

    UnarmedStance();

    switch (NewStance)
    {
    case EPlayerStance::Unarmed:
        UnarmedStance();
        GetMesh()->HideBoneByName("Gun", EPhysBodyOp::PBO_None);
        break;
    case EPlayerStance::Ranged:
        EnterRangedStance();
        GetMesh()->UnHideBoneByName("Gun");
        break;
    case EPlayerStance::Melee:
        MeleeStance();
        GetMesh()->HideBoneByName("Gun", EPhysBodyOp::PBO_None);
        break;
    default:
        break;
    }
   



}

AActor* AMainPlayer::EquipWeapon(TSubclassOf<AActor> WeaponClass, FName SocketName)
{
    if (!WeaponClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("WeaponClass is not valid."));
        return nullptr;
    }

    // 무기 클래스의 인스턴스를 생성
    AActor* NewWeapon = GetWorld()->SpawnActor<AActor>(WeaponClass, FVector::ZeroVector, FRotator::ZeroRotator);

    if (!NewWeapon)
    {
        UE_LOG(LogTemp, Warning, TEXT("NewWeapon could not be spawned."));
        return nullptr;
    }

    NewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);

    // 캐릭터의 장착된 무기 배열에 새 무기를 추가
    EquippedWeapon.AddUnique(NewWeapon);

    return NewWeapon;
}

void AMainPlayer::UnEquipAllWeapon()
{
    // 장착된 모든 무기를 순회하면서 각각을 파괴합니다.
    for (AActor* Weapon : EquippedWeapon)
    {
        if (Weapon)
        {
            Weapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform); // 필요한 경우
            Weapon->Destroy();
        }
    }

    // 배열을 비웁니다.
    EquippedWeapon.Empty();
}

bool AMainPlayer::CheckCanShoot()
{
    if (remainingbullet > 0)
        Canshoot = true;
    else
        Canshoot = false;

    return false;
}

void AMainPlayer::DodgeDirection(float distance)
{
    //개선 버전 그냥 Normaliszation 사용하는거보다 훨씬 안전함
    FVector Velocity = GetVelocity();
    FVector DodgeDirection = Velocity.GetSafeNormal(); // 방향 벡터 정규화

    if (!DodgeDirection.IsNearlyZero())
    {
        FVector StartLocation = GetActorLocation();
        FVector EndLocation = StartLocation + (DodgeDirection * distance);

        // 충돌 검사
        FHitResult HitResult;
        FCollisionQueryParams TraceParams;
        TraceParams.AddIgnoredActor(this); // 자기 자신은 충돌 감지 제외

        bool bHit = GetWorld()->LineTraceSingleByChannel(
            HitResult, StartLocation, EndLocation, ECC_Visibility, TraceParams
        );

        if (bHit)
        {
            // 벽에 부딪히면 충돌 지점까지만 이동
            EndLocation = HitResult.Location - (DodgeDirection * 10.0f); // 약간의 여유를 두어 벽에 붙지 않도록 조정
        }

        DodgeToLocation(EndLocation);
    }
}

void AMainPlayer::DodgeToLocation(const FVector& Location)
{
    if (!bHasDodged)//Do once 한번만 하려고
    {
        bHasDodged = true;

        // Effect Start
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DodgeEffect_Start, Location, FRotator::ZeroRotator, FVector(0.25f), true);

        // Effect Tail
        FVector MyLocation = GetActorLocation();
        FRotator LookAtRotation = FRotationMatrix::MakeFromX(Location - MyLocation).Rotator();
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DodgeEffect_Tail, Location, LookAtRotation, FVector(1.0f), true);

        if(CameraBoom)
        CameraBoom->bEnableCameraLag=true;

        //Effect End
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DodgeEffect_End, Location, FRotator::ZeroRotator, FVector(0.25f), true);

        SetActorLocation(Location, false, nullptr, ETeleportType::TeleportPhysics);
        UGameplayStatics::PlaySoundAtLocation(this, DodgeSound, Location);

        if (GetMesh())
        {
            GetMesh()->SetVisibility(false, true);
        }

        // 대쉬할때 무적으로 damage component 에 invincible 변수 넣어놨음
        DamageSystemComponent->SetInvincible(true);
        GetWorld()->GetTimerManager().SetTimer(DelayTimerHandle, this, &AMainPlayer::WhenActionAfterDodgeEnd, 0.2f, false);

        GetMesh()->SetVisibility(true, true);

        GetWorld()->GetTimerManager().SetTimer(DelayTimerHandle, this, &AMainPlayer::WhenBeforeResetDodge, 0.8f, false);

    }
}

void AMainPlayer::ResetDodge()
{
    bHasDodged = false;
}

void AMainPlayer::WhenActionAfterDodgeEnd()
{
    DamageSystemComponent->SetInvincible(false);//무적상태 해재
}

void AMainPlayer::WhenBeforeResetDodge()
{
    ResetDodge();
}

void AMainPlayer::OnDeath()
{
}

//------------------------------------------DisplayHUD

void AMainPlayer::DisplayHUD()
{
    if (!HUDWidgetClass) return; // HUD 클래스를 설정하지 않았다면 리턴

    // HUD 위젯이 이미 존재하는 경우, Visibility만 변경
    if (HUDWidget)
    {
        if (!HUDWidget->IsInViewport()) // Viewport에 추가되지 않았다면 추가
        {
            HUDWidget->AddToViewport();
        }

        HUDWidget->SetVisibility(ESlateVisibility::Visible); // 보이도록 설정
        return;
    }

    // HUD가 없으면 새로 생성
    HUDWidget = CreateWidget<UPlayer_HUD_CPP>(GetWorld(), HUDWidgetClass);
    if (HUDWidget)
    {
        HUDWidget->SetPlayerReference(this);
        HUDWidget->SetVisibility(ESlateVisibility::Visible); 
        HUDWidget->AddToViewport(); // Viewport에 추가
    }
}

//--------------------------------Interface---------------------------------

float AMainPlayer::GetCurrentHealth_Implementation()
{
    return DamageSystemComponent->GetCurrentHP_FromComponent();
}

float AMainPlayer::GetMaxHealth_Implementation()
{
    return DamageSystemComponent->GetMaxHP_FromComponent();
}

float AMainPlayer::Heal_Implementation(float Amount)
{
    return DamageSystemComponent->HealCPP(Amount);
}

bool AMainPlayer::IsDead_Implementation()
{
    return DamageSystemComponent->GetIsDead();
}

bool AMainPlayer::TakeDamage_Implementation(FDamageInfo& DamageInfo, AActor* DamageCauser)
{
    return DamageSystemComponent->TakeDamageCPP(DamageInfo, DamageCauser);
}

bool AMainPlayer::IsAttacking_Implementation()
{
    return attacking;
}

bool AMainPlayer::ReserveAttackToken_Implementation(int Amount)
{
    return DamageSystemComponent->ReserveAttackTokenCPP(Amount);
}

void AMainPlayer::ReturnAttackToken_Implementation(int Amount)
{
    DamageSystemComponent->ReturnAttackTokenCPP(Amount);
}

int AMainPlayer::GetTeamNumber_Implementation()
{
    return teamNumber;
}

void AMainPlayer::SetIsInterruptible_Implementation(bool Value)
{
    DamageSystemComponent->SetIsInterruptible_FromComponent(Value);
}

void AMainPlayer::SetIsInvincible_Implementation(bool value)
{
    DamageSystemComponent->SetInvincible(value);
}

//----------------------------------------------CombatStateandFight

void AMainPlayer::StartBlock()
{
    if (Stance != EPlayerStance::Unarmed)
        return;
    
    if (!bhasBlocked)//Do once 똑같이 하기!
    {
        bhasBlocked = true;

        UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
        if (AnimInstance)
        {
            float MontagePlayRate = 1.0f;
            float MontageStartPosition = 0.0f;
            UAnimMontage* MontageToPlay = MontageBlock;


            float MontageLength = AnimInstance->Montage_Play(MontageToPlay, MontagePlayRate, EMontagePlayReturnType::MontageLength, MontageStartPosition);
            bool bPlayedSuccessfully = MontageLength > 0.f;
            if (bPlayedSuccessfully)
            {

                // Bind the OnMontageEnded delegate
                FOnMontageEnded OnMontageEndedDelegate;
                OnMontageEndedDelegate.BindUObject(this, &AMainPlayer::OnMontageCompleted_Block);
                AnimInstance->Montage_SetEndDelegate(OnMontageEndedDelegate, MontageToPlay);

                // Bind to Notify Begin and End for additional actions
                AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &AMainPlayer::OnNotifyBeginReceived_Block);

                AnimInstance->OnPlayMontageNotifyEnd.AddDynamic(this, &AMainPlayer::OnNotifyEndReceived_Block);
            }
            else
            {
                // Handle montage interrupted or failed to play
                OnInterrupted_Block();
            }
        }

    }
}

void AMainPlayer::EndBlock()
{
    DamageSystemComponent->SetIsBlocking(false);
    ResetBlock();

    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (AnimInstance)
    {
        float BlendOutTime = 0.1f; // 애니메이션을 중단할 때 페이드 아웃하는 데 걸리는 시간
        AnimInstance->Montage_Stop(BlendOutTime);
    }

    IsWithinParryCombo = false;
}

void AMainPlayer::ResetBlock()
{
    bhasBlocked = false;
}

void AMainPlayer::MeleeAttack()
{
    if (Stance == EPlayerStance::Melee&&(!bMeleeAttackExcute))
    {
        
        PerformMeleeAttack();
    }

}

void AMainPlayer::ShootRangeAttack() //Same as RangeAttack
{
    if (Stance == EPlayerStance::Ranged && (!bRangeAttackExcute))
    {

        PerformRangeAttack();
    }

}

void AMainPlayer::PerformMeleeAttack()
{
    bMeleeAttackExcute = true;
    attacking = true;
    if (!isWithinResumeComboWindow)
    {
        if(GetMesh()&&SwordComboMontage01)
        {
            UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

            if (AnimInstance)
            {
                float MontagePlayRate = 1.0f;
                float MontageStartPosition = 0.0f;
                FName MontageStartingSection = NAME_None;

                //기존 바인딩 제거
                AnimInstance->OnPlayMontageNotifyBegin.RemoveDynamic(this, &AMainPlayer::OnNotifyBeginReceived_SwordCombo1);
                AnimInstance->OnPlayMontageNotifyEnd.RemoveDynamic(this, &AMainPlayer::OnNotifyEndReceived_SwordCombo1);

                // 플레이 성공적일시
                float MontageLength = AnimInstance->Montage_Play(SwordComboMontage01, MontagePlayRate, EMontagePlayReturnType::MontageLength, MontageStartPosition);
                bool bPlayedSuccessfully = MontageLength > 0.f;

                if (bPlayedSuccessfully)
                {
                    if (MontageStartingSection != "Slash")
                    {
                        AnimInstance->Montage_JumpToSection(MontageStartingSection, SwordComboMontage01);
                    }


                    // Complete 을 담당하는 EndDelegate
                    FOnMontageEnded OnMontageEndedDelegate;
                    OnMontageEndedDelegate.BindUObject(this, &AMainPlayer::OnMontageCompleted_SwordCombo);


                    //여기서 엔딩하는 신호를 보내줌
                    AnimInstance->Montage_SetEndDelegate(OnMontageEndedDelegate, SwordComboMontage01);

                    // Bind to Notify Begin and End for additional actions
                    AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &AMainPlayer::OnNotifyBeginReceived_SwordCombo1);
                    AnimInstance->OnPlayMontageNotifyEnd.AddDynamic(this, &AMainPlayer::OnNotifyEndReceived_SwordCombo1);

                }
            }
        }
    }
    else {
        bCanResumeCombo = true;
    }


}

void AMainPlayer::PerformRangeAttack()
{
    canMove = false;
    attacking = true;

    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

    if (AnimInstance)
    {
        float MontagePlayRate = 1.0f;
        float MontageStartPosition = 0.0f;
        FName MontageStartingSection = NAME_None;

        // 플레이 성공적일시
        float MontageLength = AnimInstance->Montage_Play(ShootingMontage, MontagePlayRate, EMontagePlayReturnType::MontageLength, MontageStartPosition);
        bool bPlayedSuccessfully = MontageLength > 0.f;

        if (bPlayedSuccessfully)
        {
            // Complete 을 담당하는 EndDelegate
            FOnMontageEnded OnMontageEndedDelegate;
            OnMontageEndedDelegate.BindUObject(this, &AMainPlayer::OnMontageCompleted_RangeShooting);


            //여기서 엔딩하는 신호를 보내줌
            AnimInstance->Montage_SetEndDelegate(OnMontageEndedDelegate, ShootingMontage);

            // Bind to Notify Begin and End for additional actions
            AnimInstance->OnPlayMontageNotifyBegin.RemoveDynamic(this, &AMainPlayer::OnNotifyBeginReceived_RangeShooting);
            AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &AMainPlayer::OnNotifyBeginReceived_RangeShooting);

        }
    }


}

void AMainPlayer::ResetMeleeAttack()
{
    bMeleeAttackExcute = false;
}

void AMainPlayer::ResetRangeAttack()
{
    bRangeAttackExcute = false;
}

void AMainPlayer::Reload()
{
    //Montage 작동하는 부분
    //Complete Interrupted 그리고 그냥 실행하는 부분
    float MontagePlayRate = 1.0f;
    float MontageStartPosition = 0.0f;
    FName MontageStartingSection;






    canMove = true;
    attacking = false;
    remainingbullet = maxBullet;


}

//--------------------------------------------------When Montage Animation Function------------------------------------------


void AMainPlayer::OnMontageCompleted_Block(UAnimMontage* Montage, bool bInterrupted)
{
    OnInterrupted_Block();//여기서 Interrupted 가 되던 안되던 어차피 Intterupted Block을 시행시킬거기때문에 bool 값이 어찌되든 상관없음
}

void AMainPlayer::OnNotifyBeginReceived_Block(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
    if (NotifyName == "Block")
    {
        DamageSystemComponent->SetIsBlocking(true);
    }
    if (NotifyName == "ParryWindow")
    {
        IsWithinParryCombo = true;
    }
}

void AMainPlayer::OnNotifyEndReceived_Block(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
    if (NotifyName == "ParryWindow")
    {
        IsWithinParryCombo = false;
    }
}



void AMainPlayer::OnInterrupted_Block()
{
    DamageSystemComponent->SetIsBlocking(false);
    ResetBlock();
}

//----------------------------------Stance-------------------------------

void AMainPlayer::UnarmedStance()
{
    Stance = EPlayerStance::Unarmed;
    UCharacterMovementComponent* CharMovement = GetCharacterMovement();//auto 써도 되는데 연습겸 이렇게 써보자
    if (CharMovement)
    {
        CharMovement->bOrientRotationToMovement = true;

        CharMovement->bUseControllerDesiredRotation = false;

        CharMovement->MaxWalkSpeed = defaultWalkSpeed;
    }
    StopAiming();

    UnEquipAllWeapon();
}

void AMainPlayer::MeleeStance()
{
    Stance = EPlayerStance::Melee;
    UCharacterMovementComponent* CharMovement = GetCharacterMovement();//auto 써도 되는데 연습겸 이렇게 써보자
    if (CharMovement)
    {
        CharMovement->bOrientRotationToMovement = true;

        CharMovement->bUseControllerDesiredRotation = false;

        CharMovement->MaxWalkSpeed = meleeWalkSpeed;
    }
    EquipWeapon(WeaponSwordClass, "sword_r");

}

void AMainPlayer::EnterRangedStance()
{
    Stance = EPlayerStance::Ranged;
    auto CharMovement = GetCharacterMovement();//auto 써도 되는데 연습겸 이렇게 써보자
    if (CharMovement)
    {
        CharMovement->bOrientRotationToMovement = false;

        CharMovement->bUseControllerDesiredRotation = true; 

        CharMovement->MaxWalkSpeed = magicWalkSpeed;
    }

    //여기에 PlayerHud 관련 변수 하나 만들어주고


    //Aim 확대 기능

    StartAiming();




}

void AMainPlayer::Move(const FInputActionValue& Value)
{
    // input is a Vector2D
    FVector2D MovementVector = Value.Get<FVector2D>();

    if (Controller != nullptr)
    {
        // find out which way is forward
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        // get forward vector
        const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

        // get right vector 
        const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

        // add movement 
        AddMovementInput(ForwardDirection, MovementVector.Y);
        AddMovementInput(RightDirection, MovementVector.X);

        if (readyToDodge)
        {
            DodgeDirection(300.0f);//대쉬 기능
        }
    }
}

void AMainPlayer::Look(const FInputActionValue& Value)
{
    // input is a Vector2D
    FVector2D LookAxisVector = Value.Get<FVector2D>();

    if (Controller != nullptr)
    {
        // add yaw and pitch input to controller
        AddControllerYawInput(LookAxisVector.X);
        AddControllerPitchInput(LookAxisVector.Y);
    }
}

void AMainPlayer::DodgeTriggered(const FInputActionValue& Value)
{
    readyToDodge = true;
}

void AMainPlayer::DodgeCompleted(const FInputActionValue& Value)
{
    readyToDodge = false;
}

void AMainPlayer::Slot1(const FInputActionValue& Value)
{
    ChangeStance(EPlayerStance::Unarmed);
}

void AMainPlayer::Slot2(const FInputActionValue& Value)
{
    ChangeStance(EPlayerStance::Ranged);
}

void AMainPlayer::Slot3(const FInputActionValue& Value)
{
    ChangeStance(EPlayerStance::Melee);
}

void AMainPlayer::AcceptAttack(const FInputActionValue& Value)
{
    switch (Stance)
    {
    case EPlayerStance::Unarmed:
        break;
    case EPlayerStance::Ranged:
        ShootRangeAttack();
        break;
    case EPlayerStance::Melee:
        MeleeAttack();
        break;
    default:
        break;
    }


}

//----------MontageFunction

void AMainPlayer::OnNotifyBeginReceived_SwordCombo1(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
    if (NotifyName == "Slash")
    {
        FDamageInfo ComboDamageInfo;
        ComboDamageInfo.Amount = 10;
        ComboDamageInfo.CanBeBlocked = true;
        ComboDamageInfo.CanBeParried = true;
        ComboDamageInfo.ShouldDamageInvincible = false;
        ComboDamageInfo.DamageResponse = EM_DamageResponse::HitReaction;
        ComboDamageInfo.DamageType = EM_DamageType::Melee;
        ComboDamageInfo.ShouldForceInterrupt = false;
        AttacksComponent->SphereTraceDamage(20, 50, ComboDamageInfo);


    }

    if (NotifyName == "AOESlash")
    {
        FDamageInfo AOEDamageInfo;
        AOEDamageInfo.Amount = 15;
        AOEDamageInfo.CanBeBlocked = false;
        AOEDamageInfo.CanBeParried = false;
        AOEDamageInfo.ShouldDamageInvincible = false;
        AOEDamageInfo.DamageResponse = EM_DamageResponse::None;
        AOEDamageInfo.DamageType = EM_DamageType::Explosion;
        AOEDamageInfo.ShouldForceInterrupt = true;
        AttacksComponent->AOEDamage(300, AOEDamageInfo);
    }

    if (NotifyName == "ResumeComboWindow")
    {
        isWithinResumeComboWindow = true;
        bCanResumeCombo = false;
        ResetMeleeAttack();
    }
}

void AMainPlayer::OnNotifyEndReceived_SwordCombo1(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
    if (NotifyName == "ResumeComboWindow")
    {
        isWithinResumeComboWindow = false;
        if (!bCanResumeCombo)
        {
            StopAnimMontage(SwordComboMontage01);
        }
    }
}

void AMainPlayer::OnMontageCompleted_SwordCombo(UAnimMontage* Montage, bool bInterrupted)
{
    OnInterrupted_SwordCombo();
}

void AMainPlayer::OnInterrupted_SwordCombo()
{
    ResetMeleeAttack();
    attacking = false;
    isWithinResumeComboWindow = false;
}

void AMainPlayer::OnNotifyBeginReceived_RangeShooting(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
    if (NotifyName == "Fire")
    {
        // Follow Camera의 위치와 방향을 가져옴
        FVector CameraLocation = FollowCamera->GetComponentLocation();
        FVector CameraForwardVector = FollowCamera->GetForwardVector();

        // 라인 트레이스 시작 위치와 끝 위치 설정
        FVector Start = CameraLocation;
        FVector End = Start + (CameraForwardVector * 10000.0f); // 10000 단위 거리 설정

        // HitResult 구조체 선언 (충돌 정보 저장)
        FHitResult HitResult;

        // 충돌 채널 설정
        ECollisionChannel TraceChannel = ECC_Visibility;

        // 충돌 무시 설정 (자기 자신)
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(this); // 자기 자신을 충돌 검사에서 제외

        // 라인 트레이스 수행
        bool bHit = GetWorld()->LineTraceSingleByChannel(
            HitResult, // 충돌 결과
            Start,     // 시작 위치
            End,       // 끝 위치
            TraceChannel, // 트레이스 채널
            Params     // 충돌 무시 설정
        );

        // 충돌 결과 확인
        FVector TargetLocation;

        if (bHit)
        {
            TargetLocation = HitResult.ImpactPoint; // 충돌 지점
        }
        else
        {
            TargetLocation = End; // 충돌하지 않으면 최대 거리
        }

        // 손 소켓 위치 가져오기
        USkeletalMeshComponent* MeshComponent = GetMesh();
        if (!MeshComponent)
        {
            UE_LOG(LogTemp, Warning, TEXT("Mesh Component not found!"));
            return;
        }

        FVector HandSocketLocation = MeshComponent->GetSocketLocation(FName("hand_r")); // 소켓 이름: "hand_r"

        // 방향 계산 (Find Look at Rotation)
        FRotator SpawnTransformRotation = UKismetMathLibrary::FindLookAtRotation(HandSocketLocation, TargetLocation);

        FVector SpawnTransformScale = FVector(1.0f, 1.0f, 1.0f);//스케일

        FTransform ShootingTransform(SpawnTransformRotation, HandSocketLocation, SpawnTransformScale);

        FDamageInfo ShootingDamageInfo;
        ShootingDamageInfo.Amount = 20.0f;
        ShootingDamageInfo.CanBeBlocked = true;
        ShootingDamageInfo.CanBeParried = false;
        ShootingDamageInfo.DamageResponse = EM_DamageResponse::None;
        ShootingDamageInfo.DamageType = EM_DamageType::Projectile;
        ShootingDamageInfo.ShouldForceInterrupt = false;
        ShootingDamageInfo.ShouldDamageInvincible = false;

        AttacksComponent->MagicSpell(ShootingTransform, NULL, ShootingDamageInfo);
    }
}

void AMainPlayer::OnMontageCompleted_RangeShooting(UAnimMontage* Montage, bool bInterrupted)
{
    OnInterrupted_RangeShooting();
}

void AMainPlayer::OnInterrupted_RangeShooting()
{
    canMove = true;
    attacking = false;
    ResetRangeAttack();
}

//------------------------------------Aiming 관련 함수

void AMainPlayer::StartAiming()
{

    //isAiming=true;
    if (AimTimeline)
    {
        AimTimeline->PlayFromStart();
    }
}

void AMainPlayer::StopAiming()
{
    //isAiming=false;
    if (AimTimeline)
    {
        AimTimeline->Reverse();
    }
}


void AMainPlayer::UpdateCameraOffset(float Alpha) // begin에서 바인드시켜서 카메라 오프셋 조정
{
    FVector NewOffset = FMath::Lerp(DefaultBoomOffset, AimBoomOffset, Alpha);
    CameraBoom->SocketOffset = NewOffset;
}