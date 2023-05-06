// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
    void RemoveItem(TSubclassOf<UItemStaticData> InItemStaticDataClass);
    TArray<FInventoryListItem>& GetItemsRef() { return Items; }
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
