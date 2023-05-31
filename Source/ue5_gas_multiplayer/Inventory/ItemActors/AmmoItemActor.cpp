// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoItemActor.h"

#include "Inventory/InventoryItemInstance.h"


AAmmoItemActor::AAmmoItemActor()
{
    
}

const UAmmoItemStaticData* AAmmoItemActor::GetAmmoItemStaticData() const
{
    return ItemInstance ? Cast<UAmmoItemStaticData>(ItemInstance->GetItemStaticData()) : nullptr;
}

void AAmmoItemActor::InitInternal()
{
    Super::InitInternal();

    if (const auto* AmmoData = GetAmmoItemStaticData())
    {
        if (AmmoData->StaticMesh)
        {
            if (auto* StaticMesh = NewObject<UStaticMeshComponent>(this, "MeshComponent"))
            {
                StaticMesh->RegisterComponent();
                StaticMesh->SetStaticMesh(AmmoData->StaticMesh);
                StaticMesh->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
                MeshComponent = StaticMesh;
            }
        }
    }
}
