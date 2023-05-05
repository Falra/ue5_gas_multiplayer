// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "InventoryList.generated.h"

class UInventoryItemInstance;

USTRUCT(BlueprintType)
struct FInventoryListItem : public FFastArraySerializerItem
{
    GENERATED_BODY()

protected:

    UPROPERTY()
    UInventoryItemInstance* ItemInstance = nullptr;
};

USTRUCT(USTRUCT)
struct FInventoryList : public FFastArraySerializer
{
    GENERATED_BODY()

    bool NetDeltaSerialize(FNetDeltaSerializeInfo & DeltaParams)
    {
        return FastArrayDeltaSerialize<FInventoryListItem, FInventoryList>( Items, DeltaParams, *this );
    }
    
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
