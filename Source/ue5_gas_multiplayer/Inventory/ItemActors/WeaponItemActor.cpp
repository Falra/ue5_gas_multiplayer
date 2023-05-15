// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponItemActor.h"

#include "Inventory/InventoryItemInstance.h"

AWeaponItemActor::AWeaponItemActor()
{
    
}

const UWeaponStaticData* AWeaponItemActor::GetWeaponStaticData() const
{
    return ItemInstance ? Cast<UWeaponStaticData>(ItemInstance->GetItemStaticData()) : nullptr;
}

void AWeaponItemActor::InitInternal()
{
    Super::InitInternal();

    if (const auto* WeaponData = GetWeaponStaticData())
    {
        if (WeaponData->SkeletalMesh)
        {
            if (auto* SkeletalComponent = NewObject<USkeletalMeshComponent>(this, "MeshComponent"))
            {
                SkeletalComponent->RegisterComponent();
                SkeletalComponent->SetSkeletalMesh(WeaponData->SkeletalMesh);
                SkeletalComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
                MeshComponent = SkeletalComponent;
            }
        }
        else if (WeaponData->StaticMesh)
        {
            if (auto* StaticMesh = NewObject<UStaticMeshComponent>(this, "MeshComponent"))
            {
                StaticMesh->RegisterComponent();
                StaticMesh->SetStaticMesh(WeaponData->StaticMesh);
                StaticMesh->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
                MeshComponent = StaticMesh;
            }
        }
        
    }
}
