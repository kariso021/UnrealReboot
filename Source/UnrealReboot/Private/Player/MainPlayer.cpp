// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/MainPlayer.h"
#include "TimerManager.h"
#include <GameFramework/CharacterMovementComponent.h>
#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetMathLibrary.h>

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

    
    //------------Melee Attack ���� ��

    bMeleeAttackExcute = false;
    bRangeAttackExcute = false;




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

    // ���� Ŭ������ �ν��Ͻ��� ����
    AActor* NewWeapon = GetWorld()->SpawnActor<AActor>(WeaponClass, FVector::ZeroVector, FRotator::ZeroRotator);

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
    for (AActor* Weapon : EquippedWeapon)
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

                // �÷��� �������Ͻ�
                float MontageLength = AnimInstance->Montage_Play(SwordComboMontage01, MontagePlayRate, EMontagePlayReturnType::MontageLength, MontageStartPosition);
                bool bPlayedSuccessfully = MontageLength > 0.f;

                if (bPlayedSuccessfully)
                {
                    if (MontageStartingSection != "Slash")
                    {
                        AnimInstance->Montage_JumpToSection(MontageStartingSection, SwordComboMontage01);
                    }


                    // Complete �� ����ϴ� EndDelegate
                    FOnMontageEnded OnMontageEndedDelegate;
                    OnMontageEndedDelegate.BindUObject(this, &AMainPlayer::OnMontageCompleted_SwordCombo);


                    //���⼭ �����ϴ� ��ȣ�� ������
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

        // �÷��� �������Ͻ�
        float MontageLength = AnimInstance->Montage_Play(ShootingMontage, MontagePlayRate, EMontagePlayReturnType::MontageLength, MontageStartPosition);
        bool bPlayedSuccessfully = MontageLength > 0.f;

        if (bPlayedSuccessfully)
        {
            // Complete �� ����ϴ� EndDelegate
            FOnMontageEnded OnMontageEndedDelegate;
            OnMontageEndedDelegate.BindUObject(this, &AMainPlayer::OnMontageCompleted_RangeShooting);


            //���⼭ �����ϴ� ��ȣ�� ������
            AnimInstance->Montage_SetEndDelegate(OnMontageEndedDelegate, ShootingMontage);

            // Bind to Notify Begin and End for additional actions
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
    UnEquipAllWeapon();
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
    EquipWeapon(WeaponSwordClass, "sword_r");

}

void AMainPlayer::EnterRangedStance()
{
    Stance = EPlayerStance::Ranged;
    auto CharMovement = GetCharacterMovement();//auto �ᵵ �Ǵµ� ������ �̷��� �Ẹ��
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

    }

    if (NotifyName == "AOESlash")
    {

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
        // Follow Camera�� ��ġ�� ������ ������
        FVector CameraLocation = FollowCamera->GetComponentLocation();
        FVector CameraForwardVector = FollowCamera->GetForwardVector();

        // ���� Ʈ���̽� ���� ��ġ�� �� ��ġ ����
        FVector Start = CameraLocation;
        FVector End = Start + (CameraForwardVector * 10000.0f); // 10000 ���� �Ÿ� ����

        // HitResult ����ü ���� (�浹 ���� ����)
        FHitResult HitResult;

        // �浹 ä�� ����
        ECollisionChannel TraceChannel = ECC_Visibility;

        // �浹 ���� ���� (�ڱ� �ڽ�)
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(this); // �ڱ� �ڽ��� �浹 �˻翡�� ����

        // ���� Ʈ���̽� ����
        bool bHit = GetWorld()->LineTraceSingleByChannel(
            HitResult, // �浹 ���
            Start,     // ���� ��ġ
            End,       // �� ��ġ
            TraceChannel, // Ʈ���̽� ä��
            Params     // �浹 ���� ����
        );

        // �浹 ��� Ȯ��
        FVector TargetLocation;

        if (bHit)
        {
            TargetLocation = HitResult.ImpactPoint; // �浹 ����
        }
        else
        {
            TargetLocation = End; // �浹���� ������ �ִ� �Ÿ�
        }

        // �� ���� ��ġ ��������
        USkeletalMeshComponent* MeshComponent = GetMesh();
        if (!MeshComponent)
        {
            UE_LOG(LogTemp, Warning, TEXT("Mesh Component not found!"));
            return;
        }

        FVector HandSocketLocation = MeshComponent->GetSocketLocation(FName("hand_r")); // ���� �̸�: "hand_r"

        // ���� ��� (Find Look at Rotation)
        FRotator SpawnTransformRotation = UKismetMathLibrary::FindLookAtRotation(HandSocketLocation, TargetLocation);

        FVector SpawnTransformScale = FVector(1.0f, 1.0f, 1.0f);//������

        FTransform ShootingTransform(SpawnTransformRotation, HandSocketLocation, SpawnTransformScale);

        FDamageInfo ShootingDamageInfo;
        ShootingDamageInfo.Amount = 20.0f;
        ShootingDamageInfo.CanBeBlocked = true;
        ShootingDamageInfo.CanBeParried = false;
        ShootingDamageInfo.DamageResponse = EM_DamageResponse::None;
        ShootingDamageInfo.DamageType = EM_DamageType::Projectile;
        ShootingDamageInfo.ShouldForceInterrupt = false;
        ShootingDamageInfo.ShouldDamageInvincible = false;

        AttacksComponent->MagicSpell(ShootingTransform, NULL, ShootingDamageInfo, ProjectileBullet);



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

//--------------------------Dodge

void AMainPlayer::Dodge(const FInputActionValue& Value)
{
    
}

//-----------------------------------------------------------------------------------------

