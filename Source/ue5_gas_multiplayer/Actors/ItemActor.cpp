// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemActor.h"

#include "Engine/ActorChannel.h"
#include "Inventory/InventoryItemInstance.h"
#include "Kismet/KismetSystemLibrary.h"
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

void AItemActor::Init(UInventoryItemInstance* InItemInstance)
{
    ItemInstance = InItemInstance;
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
    GetRootComponent()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
    if (AActor* Owner = GetOwner())
    {
        const FVector Location = GetActorLocation();
        const FVector Forward = Owner->GetActorForwardVector();
        const FVector TraceStart = Location + Forward * 100.0f;
        FVector TraceEnd = TraceStart * -FVector::UpVector * 1000.0f;
        TArray IgnoreActors { Owner };
        static const auto* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("AbilitySystem.ShowDebugTraversal"));
        const bool bShowTraversal = CVar->GetInt() > 0;
        const EDrawDebugTrace::Type DrawDebugType = bShowTraversal ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;
        
        FHitResult HitResult;
        UKismetSystemLibrary::LineTraceSingleByProfile(Owner, TraceStart, TraceEnd, TEXT("WorldStatic"), true, IgnoreActors,
            DrawDebugType, HitResult, true);
        if (HitResult.bBlockingHit)
        {
            TraceEnd = HitResult.Location;
        }
        SetActorLocation(TraceEnd);
    }
}

void AItemActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AItemActor, ItemInstance);
}
