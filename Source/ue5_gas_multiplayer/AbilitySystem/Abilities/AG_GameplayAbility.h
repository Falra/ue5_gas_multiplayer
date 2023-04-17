// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AG_GameplayAbility.generated.h"

UCLASS()
class UE5_GAS_MULTIPLAYER_API UAG_GameplayAbility : public UGameplayAbility
{
    GENERATED_BODY()

public:
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Effects")
    TArray<TSubclassOf<UGameplayEffect>> OngoingEffectsToRemoveOnEnd;

    UPROPERTY(EditDefaultsOnly, Category = "Effects")
    TArray<TSubclassOf<UGameplayEffect>> OngoingEffectsToJustApplyOnStart;

    TArray<FActiveGameplayEffectHandle> RemoveOnEndEffectHandles;

    UFUNCTION(BlueprintCallable, BlueprintPure)
    class AAG_Character* GetActionGameCharacterFromActorInfo() const;
};
