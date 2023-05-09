// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "InventoryItemInstance.generated.h"

class AItemActor;
class UItemStaticData;

UCLASS(BlueprintType, Blueprintable)
class UE5_GAS_MULTIPLAYER_API UInventoryItemInstance : public UObject
{
    GENERATED_BODY()

public:
    virtual void Init(TSubclassOf<UItemStaticData> InItemStaticDataClass);
    
    virtual bool IsSupportedForNetworking() const override { return true; }

    UFUNCTION(BlueprintCallable, BlueprintPure)
    const UItemStaticData* GetItemStaticData() const;

    UPROPERTY(Replicated)
    TSubclassOf<UItemStaticData> ItemStaticDataClass;

    UPROPERTY(ReplicatedUsing = OnRep_Equipped)
    bool bEquipped = false;

    UFUNCTION()
    void OnRep_Equipped();

    virtual void OnEquipped(AActor* ItemOwner = nullptr);
    virtual void OnUnequipped();

protected:

    UPROPERTY(Replicated)
    AItemActor* ItemActor;
};
