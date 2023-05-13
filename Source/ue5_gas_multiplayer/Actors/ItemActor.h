// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionGameTypes.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "ItemActor.generated.h"

class UInventoryItemInstance;
class USphereComponent;

UCLASS()
class UE5_GAS_MULTIPLAYER_API AItemActor : public AActor
{
    GENERATED_BODY()

public:
    AItemActor();
    virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
    void Init(UInventoryItemInstance* InItemInstance);
    
protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
    
    UPROPERTY(Replicated)
    UInventoryItemInstance* ItemInstance;

    UPROPERTY(Replicated)
    EItemState ItemState = EItemState::None;

    UPROPERTY()
    USphereComponent* SphereComponent;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UItemStaticData> ItemStaticDataClass;
    
public:
    virtual void Tick(float DeltaTime) override;

    virtual void OnEquipped();
    virtual void OnUnequipped();
    virtual void OnDropped();
};
