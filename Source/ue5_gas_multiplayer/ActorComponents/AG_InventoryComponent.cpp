// Fill out your copyright notice in the Description page of Project Settings.


#include "AG_InventoryComponent.h"

#include "ActionGameTypes.h"
#include "Engine/ActorChannel.h"
#include "Inventory/InventoryItemInstance.h"
#include "Net/UnrealNetwork.h"

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
}

void UAG_InventoryComponent::InitializeComponent()
{
    Super::InitializeComponent();

    for (const auto ItemClass : DefaultItems)
    {
        InventoryList.AddItem(ItemClass);
    }
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
}
