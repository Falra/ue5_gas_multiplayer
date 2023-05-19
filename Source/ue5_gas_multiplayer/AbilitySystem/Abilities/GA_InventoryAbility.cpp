// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_InventoryAbility.h"

#include "ActionGameTypes.h"
#include "ActorComponents/AG_InventoryComponent.h"
#include "Inventory/InventoryItemInstance.h"
#include "Inventory/ItemActors/WeaponItemActor.h"

void UGA_InventoryAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
    Super::OnGiveAbility(ActorInfo, Spec);

    InventoryComponent = ActorInfo->OwnerActor.Get()->FindComponentByClass<UAG_InventoryComponent>();
}

UAG_InventoryComponent* UGA_InventoryAbility::GetInventoryComponent() const
{
    return InventoryComponent;
}

UInventoryItemInstance* UGA_InventoryAbility::GetEquippedItemInstance() const
{
    return InventoryComponent ? InventoryComponent->GetEquippedItem() : nullptr;
}

const UItemStaticData* UGA_InventoryAbility::GetEquippedItemStaticData() const
{
    const UInventoryItemInstance* EquippedItem = GetEquippedItemInstance();
    return EquippedItem ? EquippedItem->GetItemStaticData() : nullptr;
}

const UWeaponStaticData* UGA_InventoryAbility::GetEquippedItemWeaponStaticData() const
{
    return Cast<UWeaponStaticData>(GetEquippedItemStaticData());
}

AItemActor* UGA_InventoryAbility::GetEquippedItemActor() const
{
    const UInventoryItemInstance* EquippedItem = GetEquippedItemInstance();
    return EquippedItem ? EquippedItem->GetItemActor() : nullptr;
}

AWeaponItemActor* UGA_InventoryAbility::GetEquippedWeaponItemActor() const
{
    return Cast<AWeaponItemActor>(GetEquippedItemActor());
}
