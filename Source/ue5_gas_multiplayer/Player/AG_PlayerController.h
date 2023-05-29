// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "AG_PlayerController.generated.h"

UCLASS()
class UE5_GAS_MULTIPLAYER_API AAG_PlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    AAG_PlayerController();

    void RestartPlayerIn(float InTime);

protected:
    virtual void OnPossess(APawn* InPawn) override;
    virtual void OnUnPossess() override;

    UFUNCTION()
    void OnPawnDeathStateChanged(const FGameplayTag CallbackTag, int32 NewCount);

    void RestartPlayer();

    FTimerHandle RestartPlayerTimerHandle;
    FDelegateHandle DeathStateTagDelegate;
};
