// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemActor.h"

#include "Engine/ActorChannel.h"
#include "Inventory/InventoryItemInstance.h"
#include "Net/UnrealNetwork.h"

AItemActor::AItemActor()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;
}

void AItemActor::BeginPlay()
{
    Super::BeginPlay();
}

bool AItemActor::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
    bool bWroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
    bWroteSomething |= Channel->ReplicateSubobject(ItemInstance, *Bunch, *RepFlags);
    return bWroteSomething;
}

void AItemActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AItemActor::OnEquipped()
{
}

void AItemActor::OnUnequipped()
{
}

void AItemActor::OnDropped()
{
}

void AItemActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AItemActor, ItemInstance);
}
