// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory/InventoryList.h"
#include "AG_InventoryComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class UE5_GAS_MULTIPLAYER_API UAG_InventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:

    UAG_InventoryComponent();

    virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override; 

    virtual void InitializeComponent() override;
    
protected:
    
    UPROPERTY(Replicated)
    FInventoryList InventoryList;

    UPROPERTY(EditDefaultsOnly)
    TArray<TSubclassOf<UItemStaticData>> DefaultItems;
    
public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

};
