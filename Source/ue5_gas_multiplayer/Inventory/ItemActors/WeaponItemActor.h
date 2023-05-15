// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/ItemActor.h"
#include "WeaponItemActor.generated.h"

UCLASS()
class UE5_GAS_MULTIPLAYER_API AWeaponItemActor : public AItemActor
{
    GENERATED_BODY()

public:
    AWeaponItemActor();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;
};
