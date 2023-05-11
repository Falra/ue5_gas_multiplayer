﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "AG_InventoryComponent.h"

#include "ActionGameTypes.h"
#include "GameplayTagsManager.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Engine/ActorChannel.h"
#include "Inventory/InventoryItemInstance.h"
#include "Net/UnrealNetwork.h"

FGameplayTag UAG_InventoryComponent::EquipItemActorTag;
FGameplayTag UAG_InventoryComponent::DropItemTag;
FGameplayTag UAG_InventoryComponent::EquipNextTag;
FGameplayTag UAG_InventoryComponent::UnequipTag;

static TAutoConsoleVariable<int32> CVarShowInventory(
    TEXT("ShowDebugInventory"),
    0,
    TEXT("Show debug info about inventory")
    TEXT("0: off\n")
    TEXT("1: on\n"),
    ECVF_Cheat
    );

UAG_InventoryComponent::UAG_InventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    bWantsInitializeComponent = true;
    SetIsReplicatedByDefault(true);

    UGameplayTagsManager::OnLastChanceToAddNativeTags().AddUObject(this, &UAG_InventoryComponent::AddInventoryTags);
}

void UAG_InventoryComponent::InitializeComponent()
{
    Super::InitializeComponent();

    if (GetOwner()->HasAuthority())
    {
        for (const auto ItemClass : DefaultItems)
        {
            InventoryList.AddItem(ItemClass);
        }
    }
}

void UAG_InventoryComponent::AddItem(TSubclassOf<UItemStaticData> InItemStaticDataClass)
{
    if (!GetOwner()->HasAuthority())
    {
        return;
    }
    InventoryList.AddItem(InItemStaticDataClass);
}

void UAG_InventoryComponent::AddItemInstance(UInventoryItemInstance* InItemInstance)
{
    if (!GetOwner()->HasAuthority())
    {
        return;
    }
    InventoryList.AddItem(InItemInstance);
}

void UAG_InventoryComponent::RemoveItem(TSubclassOf<UItemStaticData> InItemStaticDataClass)
{
    if (!GetOwner()->HasAuthority())
    {
        return;
    }
    InventoryList.RemoveItem(InItemStaticDataClass);
}

void UAG_InventoryComponent::EquipItem(TSubclassOf<UItemStaticData> InItemStaticDataClass)
{
    if (!GetOwner()->HasAuthority())
    {
        return;
    }
    for (const auto& Item : InventoryList.GetItemsRef())
    {
        if (Item.ItemInstance->ItemStaticDataClass == InItemStaticDataClass)
        {
            Item.ItemInstance->OnEquipped(GetOwner());
            CurrentItem = Item.ItemInstance;
            break;
        }
    }
}

void UAG_InventoryComponent::EquipItemInstance(UInventoryItemInstance* InItemInstance)
{
    if (!GetOwner()->HasAuthority())
    {
        return;
    }

    for (const auto& Item : InventoryList.GetItemsRef())
    {
        if (Item.ItemInstance == InItemInstance)
        {
            Item.ItemInstance->OnEquipped(GetOwner());
            CurrentItem = Item.ItemInstance;
            break;
        }
    }
    
}

void UAG_InventoryComponent::UnequipItem()
{
    if (!GetOwner()->HasAuthority() || !IsValid(CurrentItem))
    {
        return;
    }
    CurrentItem->OnUnequipped();
    CurrentItem = nullptr;
}

void UAG_InventoryComponent::DropItem()
{
    if (!GetOwner()->HasAuthority() || !IsValid(CurrentItem))
    {
        return;
    }
    CurrentItem->OnDropped();
    RemoveItem(CurrentItem->ItemStaticDataClass);
    CurrentItem = nullptr;
}

void UAG_InventoryComponent::EquipNextItem()
{
}

void UAG_InventoryComponent::GameplayEventCallBack(const FGameplayEventData* Payload)
{
    auto& TagsManager = UGameplayTagsManager::Get();

    EquipItemActorTag = TagsManager.AddNativeGameplayTag(TEXT("Event.Inventory.EquipItemActor"),
        TEXT("Equip item form item actor event"));

    DropItemTag = TagsManager.AddNativeGameplayTag(TEXT("Event.Inventory.DropItem"),
        TEXT("Drop equipped item"));

    EquipNextTag = TagsManager.AddNativeGameplayTag(TEXT("Event.Inventory.EquipNext"),
        TEXT("Try to equip next item"));

    UnequipTag = TagsManager.AddNativeGameplayTag(TEXT("Event.Inventory.Unequip"),
        TEXT("Unequip equipped item"));
    
    UGameplayTagsManager::OnLastChanceToAddNativeTags().RemoveAll(this);
}

bool UAG_InventoryComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
    bool bWroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

    for (FInventoryListItem& Item : InventoryList.GetItemsRef())
    {
        UInventoryItemInstance* ItemInstance = Item.ItemInstance;
        if (IsValid(ItemInstance))
        {
            bWroteSomething |= Channel->ReplicateSubobject(ItemInstance, *Bunch, *RepFlags);
        }
    }
    return bWroteSomething;
}

void UAG_InventoryComponent::AddInventoryTags()
{
    
}

void UAG_InventoryComponent::HandleGameplayEventInternal(FGameplayEventData Payload)
{
}

void UAG_InventoryComponent::ServerHandleGameplayEvent_Implementation(FGameplayEventData Payload)
{
}

void UAG_InventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (CVarShowInventory.GetValueOnGameThread() > 0)
    {
        for (const FInventoryListItem& Item : InventoryList.GetItemsRef())
        {
            UInventoryItemInstance* ItemInstance = Item.ItemInstance;
            if (IsValid(ItemInstance))
            {
                const UItemStaticData* ItemData = ItemInstance->GetItemStaticData();
                GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Blue, FString::Printf(TEXT("Item: %s"),
                    *ItemData->ItemName.ToString()));
            }
        }
    }
}

void UAG_InventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UAG_InventoryComponent, InventoryList);
    DOREPLIFETIME(UAG_InventoryComponent, CurrentItem);
}
