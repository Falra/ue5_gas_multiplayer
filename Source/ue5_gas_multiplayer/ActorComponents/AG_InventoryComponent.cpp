// Fill out your copyright notice in the Description page of Project Settings.


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
    if (!GetOwner()->HasAuthority())
    {
        return;
    }

    TArray<FInventoryListItem>& Items = InventoryList.GetItemsRef();
    const bool bNoItems = Items.Num() == 0;
    const bool bOneAndEquipped = Items.Num() == 1 && CurrentItem;
    if (bNoItems || bOneAndEquipped)
    {
        return;
    }

    UInventoryItemInstance* TargetItem = nullptr;
    for (const auto& Item : Items)
    {
        if (Item.ItemInstance->GetItemStaticData()->bCanBeEquipped
            && Item.ItemInstance != CurrentItem)
        {
            TargetItem = Item.ItemInstance;
            break;
        }
    }

    if (!TargetItem)
    {
        return;
    }
    
    if (CurrentItem)
    {
        UnequipItem();
    }

    EquipItemInstance(TargetItem);
}

void UAG_InventoryComponent::GameplayEventCallBack(const FGameplayEventData* Payload)
{
    const ENetRole NetRole = GetOwnerRole();
    if (NetRole == ROLE_Authority)
    {
        HandleGameplayEventInternal(*Payload);
    }
    else if (NetRole == ROLE_AutonomousProxy)
    {
        ServerHandleGameplayEvent(*Payload);
    }
}

bool UAG_InventoryComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
    bool bWroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

    for (const FInventoryListItem& Item : InventoryList.GetItemsRef())
    {
        if (UInventoryItemInstance* ItemInstance = Item.ItemInstance; IsValid(ItemInstance))
        {
            bWroteSomething |= Channel->ReplicateSubobject(ItemInstance, *Bunch, *RepFlags);
        }
    }
    return bWroteSomething;
}

void UAG_InventoryComponent::AddInventoryTags() const
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

void UAG_InventoryComponent::HandleGameplayEventInternal(const FGameplayEventData& Payload)
{
    if (!GetOwner()->HasAuthority())
    {
        return;
    }
    const FGameplayTag EventTag = Payload.EventTag;
    if (EventTag == EquipItemActorTag)
    {
        if (const auto* ItemInstance = Cast<UInventoryItemInstance>(Payload.OptionalObject))
        {
            EquipItemInstance(const_cast<UInventoryItemInstance*>(ItemInstance));
            if (Payload.Instigator)
            {
                const_cast<AActor*>(Payload.Instigator.Get())->Destroy();
            }
        }
    }
    else if (EventTag == DropItemTag)
    {
        DropItem();
    }
    else if (EventTag == EquipNextTag)
    {
        EquipNextItem();
    }
    else if (EventTag == UnequipTag)
    {
        UnequipItem();
    }
}

void UAG_InventoryComponent::ServerHandleGameplayEvent_Implementation(const FGameplayEventData Payload)
{
    HandleGameplayEventInternal(Payload);
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
