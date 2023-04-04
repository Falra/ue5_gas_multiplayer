// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionGameTypes.h"
#include "Engine/DataAsset.h"
#include "CharacterAnimDataAsset.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class UE5_GAS_MULTIPLAYER_API UCharacterAnimDataAsset : public UDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditDefaultsOnly)
    FCharacterAnimationData CharacterAnimData;
};
