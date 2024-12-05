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

    //// Forward arrow <- Arrow �� ���� ������ �̰� ������
    //ForwardArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("ForwardArrow"));
    //ForwardArrow->SetupAttachment(RootComponent);


}

// Called when the game starts or when spawned
void AMainPlayer::BeginPlay()
{
	Super::BeginPlay();

    if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        // Enhanced Input Subsystem ��������
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            // Input Mapping Context �߰�
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
        break;
    case EPlayerStance::Melee:
        MeleeStance();
        break;
    default:
        break;
    }
   



}

AWeaponBase* AMainPlayer::EquipWeapon(TSubclassOf<AWeaponBase> WeaponClass, FName SocketName)
{
    if (!WeaponClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("WeaponClass is not valid."));
        return nullptr;
    }

    // ���� Ŭ������ �ν��Ͻ��� ����
    AWeaponBase* NewWeapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass, FVector::ZeroVector, FRotator::ZeroRotator);

    if (!NewWeapon)
    {
        UE_LOG(LogTemp, Warning, TEXT("NewWeapon could not be spawned."));
        return nullptr;
    }

    NewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);

    // ĳ������ ������ ���� �迭�� �� ���⸦ �߰�
    EquippedWeapon.AddUnique(NewWeapon);

    return NewWeapon;
}

void AMainPlayer::UnEquipAllWeapon()
{
    // ������ ��� ���⸦ ��ȸ�ϸ鼭 ������ �ı��մϴ�.
    for (AWeaponBase* Weapon : EquippedWeapon)
    {
        if (Weapon)
        {
            Weapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform); // �ʿ��� ���
            Weapon->Destroy();
        }
    }

    // �迭�� ���ϴ�.
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
    if (!bHasDodged)//Do once �ѹ��� �Ϸ���
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

        // �뽬�Ҷ� �������� damage component �� invincible ���� �־����
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
    DamageSystemComponent->SetInvincible(false);//�������� ����
}

void AMainPlayer::WhenBeforeReset()
{
    ResetDodge();
}

void AMainPlayer::OnDeath()
{
}

//--------------------------------Interface---------------------------------

float AMainPlayer::GetCurrentHealth()
{
    return DamageSystemComponent->GetCurrentHP_FromComponent();
}

float AMainPlayer::GetMaxHealth()
{
    return DamageSystemComponent->GetMaxHP_FromComponent();
}

float AMainPlayer::Heal(float Amount)
{
    return DamageSystemComponent->HealCPP(Amount);
}

bool AMainPlayer::IsDead()
{
    return DamageSystemComponent->GetIsDead();
}

bool AMainPlayer::TakeDamage(FDamageInfo& DamageInfo, AActor* DamageCauser)
{
    return DamageSystemComponent->TakeDamageCPP(DamageInfo, DamageCauser);
}

bool AMainPlayer::IsAttacking()
{
    return attacking;
}

bool AMainPlayer::ReserveAttackToken(int Amount)
{
    return DamageSystemComponent->ReserveAttackTokenCPP(Amount);
}

void AMainPlayer::ReturnAttackToken(int Amount)
{
    DamageSystemComponent->ReturnAttackTokenCPP(Amount);
}

int AMainPlayer::GetTeamNumber()
{
    return teamNumber;
}

void AMainPlayer::SetIsInterruptible(bool Value)
{
    DamageSystemComponent->SetIsInterruptible_FromComponent(Value);
}

void AMainPlayer::SetIsInvincible(bool value)
{
    DamageSystemComponent->SetInvincible(value);
}

//----------------------------------------------CombatStateandFight

void AMainPlayer::StartBlock()
{
    if (Stance != EPlayerStance::Unarmed)
        return;
    
    if (!bhasBlocked)//Do once �Ȱ��� �ϱ�!
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
        float BlendOutTime = 0.1f; // �ִϸ��̼��� �ߴ��� �� ���̵� �ƿ��ϴ� �� �ɸ��� �ð�
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
    //Montage �۵��ϴ� �κ�
    //Complete Interrupted �׸��� �׳� �����ϴ� �κ�
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
    OnInterrupted_Block();//���⼭ Interrupted �� �Ǵ� �ȵǴ� ������ Intterupted Block�� �����ų�ű⶧���� bool ���� ����ǵ� �������
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
    UCharacterMovementComponent* CharMovement = GetCharacterMovement();//auto �ᵵ �Ǵµ� ������ �̷��� �Ẹ��
    if (CharMovement)
    {
        CharMovement->bOrientRotationToMovement = true;

        CharMovement->bUseControllerDesiredRotation = false;

        CharMovement->MaxWalkSpeed = defaultWalkSpeed;
    }
}

void AMainPlayer::MeleeStance()
{
    Stance = EPlayerStance::Melee;
    UCharacterMovementComponent* CharMovement = GetCharacterMovement();//auto �ᵵ �Ǵµ� ������ �̷��� �Ẹ��
    if (CharMovement)
    {
        CharMovement->bOrientRotationToMovement = true;

        CharMovement->bUseControllerDesiredRotation = false;

        CharMovement->MaxWalkSpeed = meleeWalkSpeed;
    }

}

void AMainPlayer::EnterRangedStance()
{
    Stance = EPlayerStance::Ranged;
    auto CharMovement = GetCharacterMovement();//auto �ᵵ �Ǵµ� ������ �̷��� �Ẹ��
    if (CharMovement)
    {
        CharMovement->bOrientRotationToMovement = false;

        CharMovement->bUseControllerDesiredRotation = true; 

        CharMovement->MaxWalkSpeed = meleeWalkSpeed;
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

void AMainPlayer::Dodge(const FInputActionValue& Value)
{

}

//-----------------------------------------------------------------------------------------
