// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionGameTypes.generated.h"

class UNiagaraSystem;

USTRUCT(BlueprintType)
struct FCharacterData
{
    GENERATED_BODY();
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GAS")
    TArray<TSubclassOf<class UGameplayEffect>> Effects;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GAS")
    TArray<TSubclassOf<class UGameplayAbility>> Abilities;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
    class UCharacterAnimDataAsset* CharacterAnimDataAsset;
};

USTRUCT(BlueprintType)
struct FCharacterAnimationData
{
    GENERATED_BODY();

    UPROPERTY(EditDefaultsOnly)
    UBlendSpace* MovementBlendSpace = nullptr;

    UPROPERTY(EditDefaultsOnly)
    UBlendSpace* CrouchBlendSpace = nullptr;
    
    UPROPERTY(EditDefaultsOnly)
    UAnimSequenceBase* IdleAnimationAsset = nullptr;

    UPROPERTY(EditDefaultsOnly)
    UAnimSequenceBase* CrouchIdleAnimationAsset = nullptr;
};

UENUM(BlueprintType)
enum class EFoot : uint8
{
    Left UMETA(DisplayName = "Left"),
    Right UMETA(DisplayName = "Right")
};

UCLASS(BlueprintType, Blueprintable)
class UItemStaticData : public UObject
{
    GENERATED_BODY()

public:
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FName ItemName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<class AItemActor> ItemActorClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FName AttachmentSocket = NAME_None;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    bool bCanBeEquipped = false;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FCharacterAnimationData CharacterAnimData;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TArray<TSubclassOf<UGameplayAbility>> GrantedAbilities;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TArray<TSubclassOf<UGameplayEffect>> GrantedEffects;
};

UCLASS(BlueprintType, Blueprintable)
class UWeaponStaticData : public UItemStaticData
{
    GENERATED_BODY()

public:

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<UGameplayEffect> DamageEffect;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    USkeletalMesh* SkeletalMesh;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UStaticMesh* StaticMesh;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UAnimMontage* AttackMontage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float FireRate;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float BaseDamage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    USoundBase* AttackSound;
};

UENUM(BlueprintType)
enum class EItemState : uint8
{
    None UMETA(DisplayName = "None"),
    Equipped UMETA(DisplayName = "Equipped"),
    Dropped UMETA(DisplayName = "Dropped")
};

UENUM(BlueprintType)
enum class EMovementDirectionType : uint8
{
    None UMETA(DisplayName = "None"),
    OrientToMovement UMETA(DisplayName = "Orient To Movement"),
    Strafe UMETA(DisplayName = "Strafe")
};

UCLASS(BlueprintType, Blueprintable)
class UProjectileStaticData : public UObject
{
    GENERATED_BODY()

public:

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float BaseDamage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float DamageRadius;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float GravityMultiplier = 1.0f;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float InitialSpeed = 3000.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float MaxSpeed = 3000.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UStaticMesh* ProjectileMesh;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TArray<TSubclassOf<UGameplayEffect>> DamageEffects;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TArray<TEnumAsByte<EObjectTypeQuery>> RadialDamageQueryTypes;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TEnumAsByte<ETraceTypeQuery> RadialDamageTraceType;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UNiagaraSystem* ImpactEffect;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    USoundBase* ImpactSound;
};
