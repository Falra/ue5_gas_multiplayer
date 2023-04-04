// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionGameTypes.generated.h"

USTRUCT(BlueprintType)
struct FCharacterData
{
    GENERATED_BODY();
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GAS")
    TArray<TSubclassOf<class UGameplayEffect>> Effects;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GAS")
    TArray<TSubclassOf<class UGameplayAbility>> Abilities;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GAS")
    class UCharacterAnimDataAsset* CharacterAnimDataAsset;
};

USTRUCT(BlueprintType)
struct FCharacterAnimationData
{
    GENERATED_BODY();

    UPROPERTY(EditDefaultsOnly)
    class UBlendSpace* MovementBlendSpace = nullptr;

    UPROPERTY(EditDefaultsOnly)
    class UAnimSequenceBase* IdleAnimationAsset = nullptr;
};