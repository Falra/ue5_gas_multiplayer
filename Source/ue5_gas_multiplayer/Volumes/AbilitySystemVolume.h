// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PhysicsVolume.h"
#include "AbilitySystemVolume.generated.h"

UCLASS()
class UE5_GAS_MULTIPLAYER_API AAbilitySystemVolume : public APhysicsVolume
{
    GENERATED_BODY()

public:
    AAbilitySystemVolume();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;
    
};
