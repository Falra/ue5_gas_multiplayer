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
    FGameplayEffectSpecHandle GetWeaponEffectSpec(const FHitResult& Hit);
};
