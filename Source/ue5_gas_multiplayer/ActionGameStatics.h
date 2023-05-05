// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ActionGameTypes.h"
#include "ActionGameStatics.generated.h"

UCLASS()
class UE5_GAS_MULTIPLAYER_API UActionGameStatics : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintCallable, BlueprintPure)
    static const UItemStaticData* GetItemStaticData(TSubclassOf<UItemStaticData> ItemDataClass);
};
