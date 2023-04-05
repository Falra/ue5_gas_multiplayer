// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AG_FootstepComponent.generated.h"


enum class EFoot : uint8;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UE5_GAS_MULTIPLAYER_API UAG_FootstepComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAG_FootstepComponent();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditDefaultsOnly)
    FName LeftSocketName = TEXT("LeftFootSocket");
    
    UPROPERTY(EditDefaultsOnly)
    FName RightSocketName = TEXT("RightFootSocket");

public:
    void HandleFootstep(EFoot Foot);
};
