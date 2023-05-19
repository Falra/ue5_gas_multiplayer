// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AG_GameplayAbility.h"
#include "GA_InventoryAbility.generated.h"

class UAG_InventoryComponent;
class UInventoryItemInstance;
class UItemStaticData;
class UWeaponStaticData;
class AItemActor;
class AWeaponItemActor;

UCLASS()
class UE5_GAS_MULTIPLAYER_API UGA_InventoryAbility : public UAG_GameplayAbility
{
    GENERATED_BODY()

public:

    virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

    UFUNCTION(BlueprintCallable, BlueprintPure)
    UAG_InventoryComponent* GetInventoryComponent() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    UInventoryItemInstance* GetEquippedItemInstance() const;

    UFUNCTION(BlueprintCallable, BlueprintPure)
    const UItemStaticData* GetEquippedItemStaticData() const;

    UFUNCTION(BlueprintCallable, BlueprintPure)
    const UWeaponStaticData* GetEquippedItemWeaponStaticData() const;

    UFUNCTION(BlueprintCallable, BlueprintPure)
    AItemActor* GetEquippedItemActor() const;

    UFUNCTION(BlueprintCallable, BlueprintPure)
    AWeaponItemActor* GetEquippedWeaponItemActor() const;

protected:
    
    UPROPERTY()
    UAG_InventoryComponent* InventoryComponent = nullptr;
};
