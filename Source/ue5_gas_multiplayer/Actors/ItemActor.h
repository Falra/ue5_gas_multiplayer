// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemActor.generated.h"

class UInventoryItemInstance;

UCLASS()
class UE5_GAS_MULTIPLAYER_API AItemActor : public AActor
{
    GENERATED_BODY()

public:
    AItemActor();
    virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
    
protected:
    virtual void BeginPlay() override;
    
    UPROPERTY(Replicated)
    UInventoryItemInstance* ItemInstance;
    
public:
    virtual void Tick(float DeltaTime) override;

    virtual void OnEquipped();
    virtual void OnUnequipped();
    virtual void OnDropped();
};
