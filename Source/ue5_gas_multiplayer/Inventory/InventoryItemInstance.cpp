// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryItemInstance.h"

#include "ActionGameStatics.h"
#include "Actors/ItemActor.h"
#include "Net/UnrealNetwork.h"

void UInventoryItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UInventoryItemInstance, ItemStaticDataClass);
    DOREPLIFETIME(UInventoryItemInstance, bEquipped);
    DOREPLIFETIME(UInventoryItemInstance, ItemActor);
}

void UInventoryItemInstance::Init(TSubclassOf<UItemStaticData> InItemStaticDataClass)
{
    ItemStaticDataClass = InItemStaticDataClass;
}

const UItemStaticData* UInventoryItemInstance::GetItemStaticData() const
{
    return UActionGameStatics::GetItemStaticData(ItemStaticDataClass);
}

void UInventoryItemInstance::OnRep_Equipped()
{

}

void UInventoryItemInstance::OnEquipped()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }
    FTransform Transform;
    ItemActor = World->SpawnActorDeferred<AItemActor>(GetItemStaticData()->ItemActorClass, Transform);
    ItemActor->Init(this);
    ItemActor->FinishSpawning(Transform);
}

void UInventoryItemInstance::OnUnequipped()
{
    if (IsValid(ItemActor))
    {
        ItemActor->Destroy();
        ItemActor = nullptr;
    }
}
