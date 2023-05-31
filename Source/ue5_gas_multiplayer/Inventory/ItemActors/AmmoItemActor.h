// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/ItemActor.h"
#include "AmmoItemActor.generated.h"

UCLASS()
class UE5_GAS_MULTIPLAYER_API AAmmoItemActor : public AItemActor
{
    GENERATED_BODY()

public:
    AAmmoItemActor();

    const UAmmoItemStaticData* GetAmmoItemStaticData() const;

protected:

    UPROPERTY()
    UMeshComponent* MeshComponent;

    virtual void InitInternal() override;
};
