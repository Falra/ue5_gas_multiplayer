// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "ActionGameTypes.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "AG_Character.generated.h"

class UAG_FootstepComponent;
class UAG_InventoryComponent;
class UAG_AbilitySystemComponentBase;
class UAG_AttributeSetBase;

class UGameplayEffect;
class UGameplayAbility;

struct FGameplayEffectContextHandle;
struct FOnAttributeChangeData;

UCLASS(config=Game)
class AAG_Character : public ACharacter, public IAbilitySystemInterface
{
    GENERATED_BODY()

    /** Camera boom positioning the camera behind the character */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class USpringArmComponent* CameraBoom;

    /** Follow camera */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class UCameraComponent* FollowCamera;

    /** MappingContext */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    class UInputMappingContext* DefaultMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MotionWarp, meta = (AllowPrivateAccess = "true"))
    class UAG_MotionWarpingComponent* MotionWarpingComponent;

    UPROPERTY()
    class UAG_CharacterMovementComponent* AGCharacterMovementComponent;

    /** Jump Input Action */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    class UInputAction* JumpAction;

    /** Crouch Input Action */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* CrouchAction;

    /** Sprint Input Action */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* SprintAction;
    
    /** Move Input Action */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* MoveAction;

    /** Look Input Action */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* LookAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* MoveForwardAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* MoveSideAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* LookUpAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* TurnAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* DropItemAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* EquipNextAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* UnequipItemAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* AttackAction;
    
public:
    AAG_Character(const FObjectInitializer& ObjectInitializer);

    virtual void PostInitializeComponents() override;

    bool ApplyGameplayEffectToSelf(TSubclassOf<UGameplayEffect> Effect, FGameplayEffectContextHandle InEffectContext);

    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

    UAG_FootstepComponent* GetFootstepComponent() const;

    UAG_MotionWarpingComponent* GetMotionWarpingComponent() const;

    UAG_InventoryComponent* GetInventoryComponent() const { return InventoryComponent; }

    void OnMaxMovementSpeedChanged(const FOnAttributeChangeData& ChangeData);
    
protected:
    void GiveAbilities();
    void ApplyStartupEffects();

    virtual void PossessedBy(AController* NewController) override;
    virtual void OnRep_PlayerState() override;

    UPROPERTY(EditDefaultsOnly)
    UAG_AbilitySystemComponentBase* AbilitySystemComponent;

    UPROPERTY(Transient)
    UAG_AttributeSetBase* AttributeSet;
    
    /** Called for movement input */
    void Move(const FInputActionValue& Value);

    /** Called for looking input */
    void Look(const FInputActionValue& Value);

    void MoveForward(const FInputActionValue& Value);

    void MoveSide(const FInputActionValue& Value);

    void Turn(const FInputActionValue& Value);

    void LookUp(const FInputActionValue& Value);

    void OnJumpStarted(const FInputActionValue& Value);

    void OnJumpEnded(const FInputActionValue& Value);

    void OnCrouchStarted(const FInputActionValue& Value);

    void OnCrouchEnded(const FInputActionValue& Value);

    void OnSprintStarted(const FInputActionValue& Value);

    void OnSprintEnded(const FInputActionValue& Value);

    void DropItem(const FInputActionValue& Value);
    
    void EquipNext(const FInputActionValue& Value);

    void Unequip(const FInputActionValue& Value);

    void OnAttackStarted(const FInputActionValue& Value);

    void OnAttackEnded(const FInputActionValue& Value);
    
protected:
    // APawn interface
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // To add mapping context
    virtual void BeginPlay();

public:
    /** Returns CameraBoom subobject **/
    FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
    /** Returns FollowCamera subobject **/
    FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

    UFUNCTION(BlueprintCallable)
    FCharacterData GetCharacterData() const;

    UFUNCTION(BlueprintCallable)
    void SetCharacterData(const FCharacterData& InCharacterData);

    virtual void Landed(const FHitResult& Hit) override;

    virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

    virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
protected:

    UPROPERTY(ReplicatedUsing = OnRep_CharacterData)
    FCharacterData CharacterData;

    UFUNCTION()
    void OnRep_CharacterData();

    virtual void InitFromCharacterData(const FCharacterData& InCharacterData, bool bFromReplication = false);

    UPROPERTY(EditDefaultsOnly)
    class UCharacterDataAsset* CharacterDataAsset;

    UPROPERTY(BlueprintReadOnly)
    UAG_FootstepComponent* FootstepComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated)
    UAG_InventoryComponent* InventoryComponent;

    UPROPERTY(EditDefaultsOnly)
    FGameplayTag JumpTag;

    UPROPERTY(EditDefaultsOnly)
    FGameplayTag AttackStartedTag;

    UPROPERTY(EditDefaultsOnly)
    FGameplayTag AttackEndedTag;
    
    UPROPERTY(EditDefaultsOnly)
    FGameplayTagContainer InAirTags;

    UPROPERTY(EditDefaultsOnly)
    FGameplayTagContainer CrouchTags;

    UPROPERTY(EditDefaultsOnly)
    FGameplayTagContainer SprintTags;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UGameplayEffect> CrouchStateEffect;

    FDelegateHandle MaxMovementSpeedDelegateHandle;
};
