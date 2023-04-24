// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AG_GameplayAbility.h"
#include "GA_Vault.generated.h"

/**
 * 
 */
UCLASS()
class UE5_GAS_MULTIPLAYER_API UGA_Vault : public UAG_GameplayAbility
{
    GENERATED_BODY()

public:
    UGA_Vault();

    virtual bool CommitCheck(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, FGameplayTagContainer* OptionalRelevantTags) override;

    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
    UPROPERTY(EditDefaultsOnly, Category = HorizontalTrace)
    float HorizontalTraceRadius = 30.0f;

    UPROPERTY(EditDefaultsOnly, Category = HorizontalTrace)
    float HorizontalTraceLength = 500.0f;

    UPROPERTY(EditDefaultsOnly, Category = HorizontalTrace)
    float HorizontalTraceStep = 30.0f;

    UPROPERTY(EditDefaultsOnly, Category = HorizontalTrace)
    uint8 HorizontalTraceCount = 5;

    UPROPERTY(EditDefaultsOnly, Category = VerticalTrace)
    float VerticalTraceRadius = 30.0f;

    UPROPERTY(EditDefaultsOnly, Category = VerticalTrace)
    float VerticalTraceStep = 30.0f;

    UPROPERTY(EditDefaultsOnly)
    TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;

    UPROPERTY(EditDefaultsOnly)
    UAnimMontage* VaultMontage = nullptr;

    UPROPERTY(Transient)
    class UAbilityTask_PlayMontageAndWait* VaultMontageTask = nullptr;
    
    FVector JumpToLocation;
    FVector JumpOverLocation;
    
    UPROPERTY(EditDefaultsOnly)
    TArray<TEnumAsByte<ECollisionChannel>> CollisionChannelsToIgnore;
    
};
