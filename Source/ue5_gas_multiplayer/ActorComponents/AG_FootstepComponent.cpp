// Fill out your copyright notice in the Description page of Project Settings.

#include "ActorComponents/AG_FootstepComponent.h"

#include "ue5_gas_multiplayerCharacter.h"
#include "PhysicalMaterials/AG_PhysicalMaterial.h"

UAG_FootstepComponent::UAG_FootstepComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UAG_FootstepComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UAG_FootstepComponent::HandleFootstep(EFoot Foot)
{
    const auto* Character = Cast<Aue5_gas_multiplayerCharacter>(GetOwner());
    if (!Character)
    {
        return;
    }
    const auto* Mesh = Character->GetMesh();
    if (!Mesh)
    {
        return;
    }
    FHitResult HitResult;
    FVector SocketLocation = Mesh->GetSocketLocation(Foot == EFoot::Left ? LeftSocketName : RightSocketName);
    FVector Location = SocketLocation + FVector::UpVector * 20;

    FCollisionQueryParams QueryParams;
    QueryParams.bReturnPhysicalMaterial = true;
    QueryParams.AddIgnoredActor(Character);

    if (GetWorld()->LineTraceSingleByChannel(HitResult, Location,
        Location + FVector::UpVector * - 50.0f, ECC_WorldStatic, QueryParams))
    {
        if (HitResult.bBlockingHit)
        {
            return;
        }
        auto* Material = Cast<UAG_PhysicalMaterial>(HitResult.PhysMaterial.Get());
        if (!Material)
        {
            return;
        }
        
    }
}
