// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Components/SphereComponent.h"
#include "Engine/ActorChannel.h"
#include "Inventory/InventoryItemInstance.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

AItemActor::AItemActor()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;
    SetReplicatingMovement(true);
    
    SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
    SphereComponent->SetupAttachment(RootComponent);
    SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AItemActor::OnComponentBeginOverlap);
}

void AItemActor::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())
    {
        if (!IsValid(ItemInstance) && IsValid(ItemStaticDataClass))
        {
            ItemInstance = NewObject<UInventoryItemInstance>();
            ItemInstance->Init(ItemStaticDataClass);
        }
    }

    SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    SphereComponent->SetGenerateOverlapEvents(true);
}

void AItemActor::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    FGameplayEventData PayloadData;
    PayloadData.OptionalObject = this;
    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OtherActor, OverlapGameplayTag, PayloadData);
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
    ItemState = EItemState::Equipped;

    SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    SphereComponent->SetGenerateOverlapEvents(false);
}

void AItemActor::OnUnequipped()
{
    ItemState = EItemState::None;

    SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    SphereComponent->SetGenerateOverlapEvents(false);
}

void AItemActor::OnDropped()
{
    ItemState = EItemState::Dropped;
    
    GetRootComponent()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

    if (AActor* ItemOwner = GetOwner())
    {
        const FVector Location = GetActorLocation();
        const FVector Forward = ItemOwner->GetActorForwardVector();
        const FVector TraceStart = Location + Forward * 100.0f;
        const FVector TraceEnd = TraceStart * -FVector::UpVector * 1000.0f;
        TArray IgnoreActors { ItemOwner };
        static const auto* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("ShowDebugInventory"));
        const bool bShowDebugInventory = CVar->GetInt() > 0;
        const EDrawDebugTrace::Type DrawDebugType = bShowDebugInventory ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;
        
        FHitResult HitResult;
        UKismetSystemLibrary::LineTraceSingleByProfile(Owner, TraceStart, TraceEnd, TEXT("WorldStatic"), true, IgnoreActors,
            DrawDebugType, HitResult, true);

        SetActorLocation(HitResult.bBlockingHit ? HitResult.Location : TraceEnd);
    }
    
    SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    SphereComponent->SetGenerateOverlapEvents(true);
}

void AItemActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AItemActor, ItemInstance);
    DOREPLIFETIME(AItemActor, ItemState);
}
