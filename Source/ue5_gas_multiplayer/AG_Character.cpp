// Copyright Epic Games, Inc. All Rights Reserved.

#include "AG_Character.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameplayEffectExtension.h"
#include "GameplayEffectTypes.h"
#include "AbilitySystem/AttributeSets/AG_AttributeSetBase.h"
#include "AbilitySystem/Components/AG_AbilitySystemComponentBase.h"
#include "ActorComponents/AG_CharacterMovementComponent.h"
#include "ActorComponents/AG_FootstepComponent.h"
#include "ActorComponents/AG_InventoryComponent.h"
#include "ActorComponents/AG_MotionWarpingComponent.h"
#include "DataAssets/CharacterDataAsset.h"
#include "Net/UnrealNetwork.h"


//////////////////////////////////////////////////////////////////////////
// AAG_Character

AAG_Character::AAG_Character(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer.SetDefaultSubobjectClass<UAG_CharacterMovementComponent>(CharacterMovementComponentName))
{
    // Set size for collision capsule
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

    // Don't rotate when the controller rotates. Let that just affect the camera.
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    // Configure character movement
    GetCharacterMovement()->bOrientRotationToMovement = true;            // Character moves in the direction of input...	
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

    // Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
    // instead of recompiling to adjust them
    GetCharacterMovement()->JumpZVelocity = 700.f;
    GetCharacterMovement()->AirControl = 0.35f;
    GetCharacterMovement()->MaxWalkSpeed = 500.f;
    GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
    GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
    AGCharacterMovementComponent = Cast<UAG_CharacterMovementComponent>(GetCharacterMovement());

    // Create a camera boom (pulls in towards the player if there is a collision)
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 400.0f;       // The camera follows at this distance behind the character	
    CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

    // Create a follow camera
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
    FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

    // Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
    // are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

    // Ability system
    AbilitySystemComponent = CreateDefaultSubobject<UAG_AbilitySystemComponentBase>(TEXT("AbilitySystemComponent"));
    AbilitySystemComponent->SetIsReplicated(true);
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

    AttributeSet = CreateDefaultSubobject<UAG_AttributeSetBase>(TEXT("AttributeSet"));
    FootstepComponent = CreateDefaultSubobject<UAG_FootstepComponent>(TEXT("FootstepComponent"));
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxMovementSpeedAttribute()).AddUObject(this,
        &AAG_Character::OnMaxMovementSpeedChanged);
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).AddUObject(this,
        &AAG_Character::OnHealthAttributeChanged);
    AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(TEXT("State.Ragdoll"))).AddUObject(this,
        &AAG_Character::OnRagdollTagStateChanged);

    MotionWarpingComponent = CreateDefaultSubobject<UAG_MotionWarpingComponent>(TEXT("MotionWarpingComponent"));
    InventoryComponent = CreateDefaultSubobject<UAG_InventoryComponent>(TEXT("InventoryComponent"));
    InventoryComponent->SetIsReplicated(true);
}

void AAG_Character::PostLoad()
{
    Super::PostLoad();

    if (IsValid(CharacterDataAsset))
    {
        SetCharacterData(CharacterDataAsset->CharacterData);
    }
}

bool AAG_Character::ApplyGameplayEffectToSelf(TSubclassOf<UGameplayEffect> Effect,
    FGameplayEffectContextHandle InEffectContext)
{
    if (!Effect.Get())
    {
        return false;
    }
    const FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(Effect, 1, InEffectContext);
    if (!SpecHandle.IsValid())
    {
        return false;
    }
    const FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
    return ActiveGEHandle.WasSuccessfullyApplied();
}

UAbilitySystemComponent* AAG_Character::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

UAG_FootstepComponent* AAG_Character::GetFootstepComponent() const
{
    return FootstepComponent; 
}

UAG_MotionWarpingComponent* AAG_Character::GetMotionWarpingComponent() const
{
    return MotionWarpingComponent;
}

void AAG_Character::OnMaxMovementSpeedChanged(const FOnAttributeChangeData& ChangeData)
{
    GetCharacterMovement()->MaxWalkSpeed = ChangeData.NewValue;
}

void AAG_Character::OnHealthAttributeChanged(const FOnAttributeChangeData& ChangeData)
{
    if (ChangeData.NewValue <= 0 && ChangeData.OldValue > 0)
    {
        // How to get instigator character
        
        // AAG_Character* OtherCharacter = nullptr;
        // if (ChangeData.GEModData)
        // {
        //     const FGameplayEffectContextHandle& EffectContext = ChangeData.GEModData->EffectSpec.GetEffectContext();
        //     OtherCharacter = Cast<AAG_Character>(EffectContext.GetInstigator());
        // }

        FGameplayEventData EventData;
        EventData.EventTag = ZeroHealthEventTag;

        UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, ZeroHealthEventTag, EventData);
    }
}

void AAG_Character::StartRagdoll()
{
    USkeletalMeshComponent* SkeletalMesh = GetMesh();
    if (!SkeletalMesh || SkeletalMesh->IsSimulatingPhysics())
    {
        return;
    }
    SkeletalMesh->SetCollisionProfileName(TEXT("Ragdoll"));
    SkeletalMesh->SetSimulatePhysics(true);
    SkeletalMesh->SetAllPhysicsLinearVelocity(FVector::ZeroVector);
    SkeletalMesh->SetAllPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
    SkeletalMesh->WakeAllRigidBodies();
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AAG_Character::OnRagdollTagStateChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
    if (NewCount > 0)
    {
        StartRagdoll();   
    }
}

void AAG_Character::GiveAbilities()
{
    if (HasAuthority() && AbilitySystemComponent)
    {
        for (const auto DefaultAbility : CharacterData.Abilities)
        {
            AbilitySystemComponent->GiveAbility(DefaultAbility);
        }
    }
}

void AAG_Character::ApplyStartupEffects()
{
    if (GetLocalRole() == ROLE_Authority)
    {
        FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
        EffectContext.AddSourceObject(this);
        for (const auto CharacterEffect : CharacterData.Effects)
        {
            ApplyGameplayEffectToSelf(CharacterEffect, EffectContext);
        }
    }
}

void AAG_Character::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    AbilitySystemComponent->InitAbilityActorInfo(this, this);
    GiveAbilities();
    ApplyStartupEffects();
}

void AAG_Character::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();
    
    AbilitySystemComponent->InitAbilityActorInfo(this, this);
}

void AAG_Character::BeginPlay()
{
    // Call the base class  
    Super::BeginPlay();

    //Add Input Mapping Context
    if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
            PlayerController->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// Input

void AAG_Character::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
    // Set up action bindings
    if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
    {

        //Jumping
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AAG_Character::OnJumpStarted);
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AAG_Character::OnJumpEnded);

        //Crouch
        EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &AAG_Character::OnCrouchStarted);
        EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &AAG_Character::OnCrouchEnded);

        //Sprint
        EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AAG_Character::OnSprintStarted);
        EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AAG_Character::OnSprintEnded);
        
        //Moving
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAG_Character::Move);
        
        EnhancedInputComponent->BindAction(MoveForwardAction, ETriggerEvent::Triggered, this, &AAG_Character::MoveForward);
        EnhancedInputComponent->BindAction(MoveSideAction, ETriggerEvent::Triggered, this, &AAG_Character::MoveSide);

        //Looking
        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AAG_Character::Look);
        
        EnhancedInputComponent->BindAction(LookUpAction, ETriggerEvent::Triggered, this, &AAG_Character::LookUp);
        EnhancedInputComponent->BindAction(TurnAction, ETriggerEvent::Triggered, this, &AAG_Character::Turn);

        // Inventory
        EnhancedInputComponent->BindAction(DropItemAction, ETriggerEvent::Triggered, this, &AAG_Character::DropItem);
        EnhancedInputComponent->BindAction(EquipNextAction, ETriggerEvent::Triggered, this, &AAG_Character::EquipNext);
        EnhancedInputComponent->BindAction(UnequipItemAction, ETriggerEvent::Triggered, this, &AAG_Character::Unequip);

        // Attack
        EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &AAG_Character::OnAttackStarted);
        EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Completed, this, &AAG_Character::OnAttackEnded);

        // Aim
        EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &AAG_Character::OnAimStarted);
        EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &AAG_Character::OnAimEnded);
    }

}

void AAG_Character::Move(const FInputActionValue& Value)
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

void AAG_Character::Look(const FInputActionValue& Value)
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

void AAG_Character::MoveForward(const FInputActionValue& Value)
{
    const float MovementValue = Value.GetMagnitude();
    
    if (Controller != nullptr)
    {
        // find out which way is forward
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        // get forward vector
        const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

        // add movement 
        AddMovementInput(ForwardDirection, MovementValue);
    }
}

void AAG_Character::MoveSide(const FInputActionValue& Value)
{
    const float MovementValue = Value.GetMagnitude();
    
    if (Controller != nullptr)
    {
        // find out which way is forward
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        // get right vector 
        const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

        // add movement 
        AddMovementInput(RightDirection, MovementValue);
    }
}

void AAG_Character::Turn(const FInputActionValue& Value)
{
    const float TurnAmount = Value.GetMagnitude();
    
    if (Controller != nullptr)
    {
        AddControllerYawInput(TurnAmount);
    }
}

void AAG_Character::LookUp(const FInputActionValue& Value)
{
    const float LookAmount = Value.GetMagnitude();
    
    if (Controller != nullptr)
    {
        AddControllerPitchInput(LookAmount);
    }
}

void AAG_Character::OnJumpStarted(const FInputActionValue& Value)
{
    // FGameplayEventData Payload;
    //
    // Payload.Instigator = this;
    // Payload.EventTag = JumpTag;
    //
    // UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, JumpTag, Payload);

    AGCharacterMovementComponent->TryTraversal(AbilitySystemComponent);
}

void AAG_Character::OnJumpEnded(const FInputActionValue& Value)
{
    // StopJumping();
}

void AAG_Character::OnCrouchStarted(const FInputActionValue& Value)
{
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->TryActivateAbilitiesByTag(CrouchTags, true);
    }
}

void AAG_Character::OnCrouchEnded(const FInputActionValue& Value)
{
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->CancelAbilities(&CrouchTags);
    }
}

void AAG_Character::OnSprintStarted(const FInputActionValue& Value)
{
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->TryActivateAbilitiesByTag(SprintTags, true);
    }
}

void AAG_Character::OnSprintEnded(const FInputActionValue& Value)
{
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->CancelAbilities(&SprintTags);
    }
}

void AAG_Character::DropItem(const FInputActionValue& Value)
{
    FGameplayEventData EventData;
    EventData.EventTag = UAG_InventoryComponent::DropItemTag;

    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, UAG_InventoryComponent::DropItemTag, EventData);
}

void AAG_Character::EquipNext(const FInputActionValue& Value)
{
    FGameplayEventData EventData;
    EventData.EventTag = UAG_InventoryComponent::EquipNextTag;

    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, UAG_InventoryComponent::EquipNextTag, EventData);
}

void AAG_Character::Unequip(const FInputActionValue& Value)
{
    FGameplayEventData EventData;
    EventData.EventTag = UAG_InventoryComponent::UnequipTag;

    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, UAG_InventoryComponent::UnequipTag, EventData);
}

void AAG_Character::OnAttackStarted(const FInputActionValue& Value)
{
    FGameplayEventData EventData;
    EventData.EventTag = AttackStartedTag;

    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, AttackStartedTag, EventData);
}

void AAG_Character::OnAttackEnded(const FInputActionValue& Value)
{
    FGameplayEventData EventData;
    EventData.EventTag = AttackEndedTag;

    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, AttackEndedTag, EventData);
}

void AAG_Character::OnAimStarted(const FInputActionValue& Value)
{
    FGameplayEventData EventData;
    EventData.EventTag = AimStartedTag;

    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, AimStartedTag, EventData);
}

void AAG_Character::OnAimEnded(const FInputActionValue& Value)
{
    FGameplayEventData EventData;
    EventData.EventTag = AimEndedTag;

    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, AimEndedTag, EventData);
}

FCharacterData AAG_Character::GetCharacterData() const
{
    return CharacterData;
}

void AAG_Character::SetCharacterData(const FCharacterData& InCharacterData)
{
    CharacterData = InCharacterData;

    InitFromCharacterData(CharacterData);
}

void AAG_Character::Landed(const FHitResult& Hit)
{
    Super::Landed(Hit);

    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->RemoveActiveEffectsWithTags(InAirTags);
    }
}

void AAG_Character::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
    Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

    if (!AbilitySystemComponent || !CrouchStateEffect.Get())
    {
        return;
    }
    const FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
    FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(CrouchStateEffect, 1, EffectContext);
    if (!SpecHandle.IsValid())
    {
        return;;
    }
    FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
    if (!ActiveGEHandle.WasSuccessfullyApplied())
    {
        UE_LOG(LogTemp, Log, TEXT("Ability %s failed to apply startup effect %s"), *GetName(), *GetNameSafe(CrouchStateEffect));
    }
}

void AAG_Character::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
    if (AbilitySystemComponent && CrouchStateEffect.Get())
    {
        AbilitySystemComponent->RemoveActiveGameplayEffectBySourceEffect(CrouchStateEffect, AbilitySystemComponent);
    }
    
    Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

void AAG_Character::InitFromCharacterData(const FCharacterData& InCharacterData, bool bFromReplication)
{
}

void AAG_Character::OnRep_CharacterData()
{
    InitFromCharacterData(CharacterData, true);
}

void AAG_Character::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AAG_Character, CharacterData);
    DOREPLIFETIME(AAG_Character, InventoryComponent);
}