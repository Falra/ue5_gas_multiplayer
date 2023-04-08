// Fill out your copyright notice in the Description page of Project Settings.

#include "ActorComponents/AG_FootstepComponent.h"

#include "..\AG_Character.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicalMaterials/AG_PhysicalMaterial.h"

static TAutoConsoleVariable<int32> CVarShowFootsteps(
    TEXT("ShowDedugFootsteps"),
    0,
    TEXT("Show debug footsteps"),
    ECVF_Cheat
);

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
    const auto* Character = Cast<AAG_Character>(GetOwner());
    if (!Character)
    {
        return;
    }
    const auto* Mesh = Character->GetMesh();
    if (!Mesh)
    {
        return;
    }
    const int32 ShowFootsteps = CVarShowFootsteps.GetValueOnAnyThread();

    FVector SocketLocation = Mesh->GetSocketLocation(Foot == EFoot::Left ? LeftSocketName : RightSocketName);
    FVector Location = SocketLocation + FVector::UpVector * 20;
    FVector EndLocation = Location + FVector::UpVector * -50.0f;

    FCollisionQueryParams QueryParams;
    QueryParams.bReturnPhysicalMaterial = true;
    QueryParams.AddIgnoredActor(Character);

    if (FHitResult HitResult; GetWorld()->LineTraceSingleByChannel(HitResult, Location,
        EndLocation, ECC_WorldStatic, QueryParams))
    {
        if (HitResult.bBlockingHit)
        {
            if (ShowFootsteps > 0)
            {
                DrawDebugLine(GetWorld(), Location, EndLocation, FColor::Green, false, 4.0f, 0, 1.0f);
            }
            return;
        }
        if (auto* Material = Cast<UAG_PhysicalMaterial>(HitResult.PhysMaterial.Get()))
        {
            UGameplayStatics::PlaySoundAtLocation(this, Material->FootstepSound, SocketLocation, 1.0f);
            if (ShowFootsteps > 0)
            {
                DrawDebugString(GetWorld(), HitResult.ImpactPoint, *Material->GetName(), nullptr, FColor::White, 4.0f);
            }
        }
        if (ShowFootsteps > 0)
        {
            DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 16, 16, FColor::Red, false, 4.0f);
        }
    }
    else
    {
        if (ShowFootsteps > 0)
        {
            DrawDebugLine(GetWorld(), Location, EndLocation, FColor::Green, false, 4.0f, 0, 1.0f);
            DrawDebugSphere(GetWorld(), EndLocation, 16, 16, FColor::Red, false, 4.0f);
        }
    }
}
