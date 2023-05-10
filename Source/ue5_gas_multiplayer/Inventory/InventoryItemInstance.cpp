// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryItemInstance.h"

#include "ActionGameStatics.h"
#include "Actors/ItemActor.h"
#include "GameFramework/Character.h"
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

void UInventoryItemInstance::OnEquipped(AActor* ItemOwner /* = nullptr */)
{
    if (!ItemOwner)
    {
        return;
    }
    
    UWorld* World = ItemOwner->GetWorld();
    if (!World)
    {
        return;
    }
    FTransform Transform;
    const UItemStaticData* ItemStaticData = GetItemStaticData();
    ItemActor = World->SpawnActorDeferred<AItemActor>(ItemStaticData->ItemActorClass, Transform, ItemOwner);
    ItemActor->Init(this);
    ItemActor->FinishSpawning(Transform);
    if (const auto* Character = Cast<ACharacter>(ItemOwner))
    {
        USkeletalMeshComponent* SkeletalMesh = Character->GetMesh();
        ItemActor->AttachToComponent(SkeletalMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, ItemStaticData->AttachmentSocket);
    }
}

void UInventoryItemInstance::OnUnequipped()
{
    if (IsValid(ItemActor))
    {
        ItemActor->Destroy();
        ItemActor = nullptr;
    }
}

void UInventoryItemInstance::OnDropped()
{
    if (IsValid(ItemActor))
    {
        ItemActor->OnDropped();
    }
}
