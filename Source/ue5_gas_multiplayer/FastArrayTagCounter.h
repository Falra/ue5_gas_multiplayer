// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "UObject/Object.h"
#include "FastArrayTagCounter.generated.h"

USTRUCT(BlueprintType)
struct FFastArrayTagCounterRecord : public FFastArraySerializerItem
{
    GENERATED_BODY()
    
    UPROPERTY()
    FGameplayTag Tag;

    UPROPERTY()
    int32 Count = 0;
};

USTRUCT(BlueprintType)
struct FFastArrayTagCounter : public FFastArraySerializer
{
    GENERATED_BODY()
    
    bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
    {
        return FastArrayDeltaSerialize<FFastArrayTagCounterRecord, FFastArrayTagCounter>( TagArray, DeltaParams, *this );
    }

    int32 GetTagCount(FGameplayTag Tag) const;
    
    void AddTagCount(FGameplayTag Tag, int32 Count);
    
    const TArray<FFastArrayTagCounterRecord>& GetTagArray() const;
    
protected:

    UPROPERTY()
    TArray<FFastArrayTagCounterRecord> TagArray;
};

template<>
struct TStructOpsTypeTraits<FFastArrayTagCounter> : public TStructOpsTypeTraitsBase2<FFastArrayTagCounter>
{
    enum
    {
        WithNetDeltaSerializer = true,
    };
};
