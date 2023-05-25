// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponItemActor.h"

#include "NiagaraFunctionLibrary.h"
#include "Inventory/InventoryItemInstance.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicalMaterials/AG_PhysicalMaterial.h"

AWeaponItemActor::AWeaponItemActor()
{
    
}

const UWeaponStaticData* AWeaponItemActor::GetWeaponStaticData() const
{
    return ItemInstance ? Cast<UWeaponStaticData>(ItemInstance->GetItemStaticData()) : nullptr;
}

FVector AWeaponItemActor::GetMuzzleLocation() const
{
    return MeshComponent ? MeshComponent->GetSocketLocation(TEXT("Muzzle")) : FVector::ZeroVector;
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

void AWeaponItemActor::PlayWeaponEffects(const FHitResult& HitResult) const
{
    if (HasAuthority())
    {
        MulticastPlayWeaponEffects(HitResult);
    }
    else
    {
        PlayWeaponEffectsInternal(HitResult);
    }
}

void AWeaponItemActor::MulticastPlayWeaponEffects_Implementation(const FHitResult& HitResult) const
{
    if (!Owner || Owner->GetLocalRole() != ROLE_AutonomousProxy)
    {
        PlayWeaponEffectsInternal(HitResult);
    }
}

void AWeaponItemActor::PlayWeaponEffectsInternal(const FHitResult& HitResult) const
{
    if (HitResult.PhysMaterial.Get())
    {
        if (const auto* PhysicalMaterial = Cast<UAG_PhysicalMaterial>(HitResult.PhysMaterial.Get()))
        {
            UGameplayStatics::PlaySoundAtLocation(this, PhysicalMaterial->PointImpactSound, HitResult.ImpactPoint);
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, PhysicalMaterial->PointImpactEffect, HitResult.ImpactPoint);
        }
    }
    if (const auto* WeaponData = GetWeaponStaticData())
    {
        UGameplayStatics::PlaySoundAtLocation(this, WeaponData->AttackSound, GetActorLocation());
    }
}
