// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GA_InventoryAbility.h"
#include "GA_InventoryCombatAbility.generated.h"

/**
 * 
 */
UCLASS()
class UE5_GAS_MULTIPLAYER_API UGA_InventoryCombatAbility : public UGA_InventoryAbility
{
    GENERATED_BODY()

public:

    virtual bool CommitAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, FGameplayTagContainer* OptionalRelevantTags) override;
    
    UFUNCTION(BlueprintPure, BlueprintCallable)
    FGameplayEffectSpecHandle GetWeaponEffectSpec(const FHitResult& Hit);

    UFUNCTION(BlueprintPure, BlueprintCallable)
    const bool GetWeaponToFocusTraceResult(float TraceDistance, ETraceTypeQuery TraceType, FHitResult& OutHitResult) const;

    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool HasEnoughAmmo() const;

    UFUNCTION(BlueprintCallable)
    void DecAmmo();
};
