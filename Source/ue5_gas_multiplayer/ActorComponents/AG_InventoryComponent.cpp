// Fill out your copyright notice in the Description page of Project Settings.


#include "AG_InventoryComponent.h"

#include "Engine/ActorChannel.h"
#include "Inventory/InventoryItemInstance.h"
#include "Net/UnrealNetwork.h"

UAG_InventoryComponent::UAG_InventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    bWantsInitializeComponent = true;
    SetIsReplicatedByDefault(true);
}

void UAG_InventoryComponent::InitializeComponent()
{
    Super::InitializeComponent();
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
}

void UAG_InventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UAG_InventoryComponent, InventoryList);
}