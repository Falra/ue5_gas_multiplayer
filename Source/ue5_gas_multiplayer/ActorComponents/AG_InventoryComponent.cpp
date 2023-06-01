// Fill out your copyright notice in the Description page of Project Settings.


#include "AG_InventoryComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
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

    static bool bHandledAddingTags = false;
    if (!bHandledAddingTags)
    {
        bHandledAddingTags = true;
        UGameplayTagsManager::OnLastChanceToAddNativeTags().AddUObject(this, &UAG_InventoryComponent::AddInventoryTags);
    }
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

    if (auto* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()))
    {
        ASC->GenericGameplayEventCallbacks.FindOrAdd(EquipItemActorTag).AddUObject(this, &UAG_InventoryComponent::GameplayEventCallback);
        ASC->GenericGameplayEventCallbacks.FindOrAdd(DropItemTag).AddUObject(this, &UAG_InventoryComponent::GameplayEventCallback);
        ASC->GenericGameplayEventCallbacks.FindOrAdd(EquipNextTag).AddUObject(this, &UAG_InventoryComponent::GameplayEventCallback);
        ASC->GenericGameplayEventCallbacks.FindOrAdd(UnequipTag).AddUObject(this, &UAG_InventoryComponent::GameplayEventCallback);
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

    TArray<UInventoryItemInstance*> Items = InventoryList.GetAllAvailableInstancesOfType(InItemInstance->ItemStaticDataClass);
    
    Algo::Sort(Items, [](auto* InA, auto* InB)
    {
       return InA->GetQuantity() < InB->GetQuantity(); 
    });

    const int32 MaxItemStackCount = InItemInstance->GetItemStaticData()->MaxStackCount;
    int32 ItemsLeft = InItemInstance->GetQuantity();
    for (auto* Item : Items)
    {
        const int32 EmptySlots = MaxItemStackCount - Item->GetQuantity();
        const int32 SlotsToAdd = FMath::Max(EmptySlots, ItemsLeft);
        ItemsLeft -= SlotsToAdd;
        Item->AddItems(SlotsToAdd);
        InItemInstance->AddItems(-SlotsToAdd);
        for (const auto InventoryTag : Item->GetItemStaticData()->InventoryTags)
        {
            InventoryTags.AddTagCount(InventoryTag, SlotsToAdd);
        }
        
        if (ItemsLeft <= 0)
        {
            break;
        }
    }
    if (ItemsLeft > 0)
    {
        InventoryList.AddItem(InItemInstance);
        for (const auto InventoryTag : InItemInstance->GetItemStaticData()->InventoryTags)
        {
            InventoryTags.AddTagCount(InventoryTag, InItemInstance->GetQuantity());
        }
    }
}

void UAG_InventoryComponent::RemoveItem(TSubclassOf<UItemStaticData> InItemStaticDataClass)
{
    if (!GetOwner()->HasAuthority())
    {
        return;
    }
    InventoryList.RemoveItem(InItemStaticDataClass);
}

void UAG_InventoryComponent::RemoveItemInstance(UInventoryItemInstance* InItemInstance)
{
    if (!GetOwner()->HasAuthority())
    {
        return;
    }

    InventoryList.RemoveItem(InItemInstance);

    for (const auto InventoryTag : InItemInstance->GetItemStaticData()->InventoryTags)
    {
        InventoryTags.AddTagCount(InventoryTag, -InItemInstance->GetQuantity());
    }
}

void UAG_InventoryComponent::RemoveItemWithInventoryTag(FGameplayTag Tag, int32 Count)
{
    if (!GetOwner()->HasAuthority())
    {
        return;
    }
    int32 ItemsLeft = Count;
    TArray<UInventoryItemInstance*> Items = GetAllInstancesWithTag(Tag);

    Algo::Sort(Items, [](auto* InA, auto* InB)
    {
        return InA->GetQuantity() < InB->GetQuantity();
    });

    for (auto* Item : Items)
    {
        const int32 AvailableCount = Item->GetQuantity();
        const int32 SlotsToRemove = FMath::Min(AvailableCount, ItemsLeft);
        if (SlotsToRemove >= AvailableCount)
        {
            RemoveItemInstance(Item);
        }
        else
        {
            Item->AddItems(-SlotsToRemove);
            for (const auto InventoryTag : Item->GetItemStaticData()->InventoryTags)
            {
                InventoryTags.AddTagCount(InventoryTag, -SlotsToRemove);
            }
        }
        ItemsLeft -= SlotsToRemove;
        if (ItemsLeft <= 0)
        {
            break;
        }
    }
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
    CurrentItem->OnUnequipped(GetOwner());
    CurrentItem = nullptr;
}

void UAG_InventoryComponent::DropItem()
{
    if (!GetOwner()->HasAuthority() || !IsValid(CurrentItem))
    {
        return;
    }
    CurrentItem->OnDropped(GetOwner());
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

void UAG_InventoryComponent::GameplayEventCallback(const FGameplayEventData* Payload)
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

int32 UAG_InventoryComponent::GetInventoryTagCount(FGameplayTag Tag) const
{
    return InventoryTags.GetTagCount(Tag);
}

void UAG_InventoryComponent::AddInventoryTagCount(FGameplayTag Tag, int32 Count)
{
    InventoryTags.AddTagCount(Tag, Count);
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

TArray<UInventoryItemInstance*> UAG_InventoryComponent::GetAllInstancesWithTag(FGameplayTag Tag)
{
    return InventoryList.GetAllInstancesWithTag(Tag);
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
            AddItemInstance(const_cast<UInventoryItemInstance*>(ItemInstance));
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
            if (const UInventoryItemInstance* ItemInstance = Item.ItemInstance; IsValid(ItemInstance))
            {
                const UItemStaticData* ItemData = ItemInstance->GetItemStaticData();
                GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Blue, FString::Printf(TEXT("Item: %s"),
                    *ItemData->ItemName.ToString()));
            }
        }

        const TArray<FFastArrayTagCounterRecord>& InventoryTagArray = InventoryTags.GetTagArray();
        for (const auto& Record : InventoryTagArray)
        {
            GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Purple, FString::Printf(TEXT("Tag %s count: %d"),
                *Record.Tag.ToString(), Record.Count));
        }
    }
}

void UAG_InventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UAG_InventoryComponent, InventoryList);
    DOREPLIFETIME(UAG_InventoryComponent, CurrentItem);
    DOREPLIFETIME(UAG_InventoryComponent, InventoryTags);
}
