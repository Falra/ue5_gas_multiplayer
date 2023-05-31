// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "InventoryList.generated.h"

class UInventoryItemInstance;
class UItemStaticData;

USTRUCT(BlueprintType)
struct FInventoryListItem : public FFastArraySerializerItem
{
    GENERATED_BODY()
    
    UPROPERTY()
    UInventoryItemInstance* ItemInstance = nullptr;
};

USTRUCT(BlueprintType)
struct FInventoryList : public FFastArraySerializer
{
    GENERATED_BODY()

    bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
    {
        return FastArrayDeltaSerialize<FInventoryListItem, FInventoryList>( Items, DeltaParams, *this );
    }

    void AddItem(TSubclassOf<UItemStaticData> InItemStaticDataClass);
    void AddItem(UInventoryItemInstance* InItemInstance);
    
    void RemoveItem(TSubclassOf<UItemStaticData> InItemStaticDataClass);
    void RemoveItem(UInventoryItemInstance* InItemInstance);
    
    TArray<FInventoryListItem>& GetItemsRef() { return Items; }

    TArray<UInventoryItemInstance*> GetAllInstancesWithTag(FGameplayTag);

    TArray<UInventoryItemInstance*> GetAllAvailableInstancesOfType(TSubclassOf<UItemStaticData> InItemStaticDataClass);
    
protected:

    UPROPERTY()
    TArray<FInventoryListItem> Items;
};

template<>
struct TStructOpsTypeTraits<FInventoryList> : public TStructOpsTypeTraitsBase2<FInventoryList>
{
    enum
    {
        WithNetDeltaSerializer = true,
    };
};
