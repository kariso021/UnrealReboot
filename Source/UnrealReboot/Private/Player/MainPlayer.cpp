// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/MainPlayer.h"
#include "TimerManager.h"
#include <GameFramework/CharacterMovementComponent.h>
#include <Kismet/GameplayStatics.h>

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

    //// Forward arrow <- Arrow 가 제일 문제임 이거 넣지마
    //ForwardArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("ForwardArrow"));
    //ForwardArrow->SetupAttachment(RootComponent);


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
	
}

void AMainPlayer::OnConstruction(const FTransform& Transform)
{
    if (DamageSystemComponent)
    {
        DamageSystemComponent->SetAttackTokenCount(1);
    }
    if (GetMesh())
    {
        const FName BoneToHide = TEXT("BoneNameHere");
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


    }


}


void AMainPlayer::ChangeStance(EPlayerStance NewStance)
{
    if (Stance == NewStance)
        return;
    EPlayerStance OldStance;
    OldStance = Stance; 
    Stance = NewStance; 

    switch (NewStance)
    {
    case EPlayerStance::Unarmed:
        UnarmedStance();
        break;
    case EPlayerStance::Ranged:
        EnterRangedStance();
        UE_LOG(LogTemp, Display, TEXT("RangeStance !"));
        break;
    case EPlayerStance::Melee:
        MeleeStance();
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
    FVector Velocity = GetVelocity();
    Velocity.Normalize();

    FVector ActorLocation = GetActorLocation();
    FVector TargetLocation = ActorLocation + Velocity * distance;

    DodgeToLocation(TargetLocation);
}

void AMainPlayer::DodgeToLocation(const FVector& Location)
{
    if (!bHasDodged)//Do once 한번만 하려고
    {
        bHasDodged = true;

        // Effect Start
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DodgeEffect1, Location, FRotator::ZeroRotator, FVector(0.25f), true);

        // Effect Tail
        FVector MyLocation = GetActorLocation();
        FRotator LookAtRotation = FRotationMatrix::MakeFromX(Location - MyLocation).Rotator();
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DodgeEffect2, Location, LookAtRotation, FVector(1.0f), true);

        if(CameraBoom)
        CameraBoom->bEnableCameraLag=true;

        //Effect End
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DodgeEffect3, Location, FRotator::ZeroRotator, FVector(0.25f), true);


        UGameplayStatics::PlaySoundAtLocation(this, DodgeSound, Location);

        if (GetMesh())
        {
            GetMesh()->SetVisibility(false, true);
        }

        // 대쉬할때 무적으로 damage component 에 invincible 변수 넣어놨음
        DamageSystemComponent->SetInvincible(true);
        GetWorld()->GetTimerManager().SetTimer(DelayTimerHandle, this, &AMainPlayer::WhenActionAfterDodgeEnd, 0.2f, false);

        GetMesh()->SetVisibility(true, true);

        GetWorld()->GetTimerManager().SetTimer(DelayTimerHandle, this, &AMainPlayer::WhenBeforeReset, 0.8f, false);

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

void AMainPlayer::WhenBeforeReset()
{
    ResetDodge();
}

void AMainPlayer::OnDeath()
{
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

}

void AMainPlayer::ResetMeleeAttack()
{

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
    EquipWeapon(WeaponSwordClass, "hand_r_sword_socket");

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

void AMainPlayer::Slot1(const FInputActionValue& Value)
{
    UE_LOG(LogTemp, Display, TEXT("Slot1 Selected"));
    ChangeStance(EPlayerStance::Unarmed);
}

void AMainPlayer::Slot2(const FInputActionValue& Value)
{
    UE_LOG(LogTemp, Display, TEXT("Slot2 Selected"));
    ChangeStance(EPlayerStance::Ranged);
}

void AMainPlayer::Slot3(const FInputActionValue& Value)
{
    UE_LOG(LogTemp, Display, TEXT("Slot3 Selected"));
    ChangeStance(EPlayerStance::Melee);
}

void AMainPlayer::Dodge(const FInputActionValue& Value)
{

}

//-----------------------------------------------------------------------------------------

