// Fill out your copyright notice in the Description page of Project Settings.


#include "AG_PlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AG_GameMode.h"


AAG_PlayerController::AAG_PlayerController()
{
}

void AAG_PlayerController::RestartPlayerIn(float InTime)
{
    ChangeState(NAME_Spectating);
    GetWorldTimerManager().SetTimer(RestartPlayerTimerHandle, this, &AAG_PlayerController::RestartPlayer, InTime);
}

void AAG_PlayerController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    if (auto* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InPawn))
    {
        DeathStateTagDelegate = ASC->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(TEXT("State.Dead"))).AddUObject(this,
            &AAG_PlayerController::OnPawnDeathStateChanged);
    }
}

void AAG_PlayerController::OnUnPossess()
{
    if (DeathStateTagDelegate.IsValid())
    {
        if (auto* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn()))
        {
            ASC->UnregisterGameplayTagEvent(DeathStateTagDelegate, FGameplayTag::RequestGameplayTag(TEXT("State.Dead")));
        }
    }
    Super::OnUnPossess();
}

void AAG_PlayerController::OnPawnDeathStateChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
    if (NewCount > 0)
    {
        const UWorld* World = GetWorld();
        if (auto* GameMode = World ? Cast<AAG_GameMode>(World->GetAuthGameMode()) : nullptr)
        {
            GameMode->NotifyPlayerDied(this);
        }

        if (DeathStateTagDelegate.IsValid())
        {
            if (auto* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn()))
            {
                ASC->UnregisterGameplayTagEvent(DeathStateTagDelegate, FGameplayTag::RequestGameplayTag(TEXT("State.Dead")));
            }
        }
    }
}

void AAG_PlayerController::RestartPlayer()
{
    const UWorld* World = GetWorld();
    if (auto* GameMode = World ? Cast<AAG_GameMode>(World->GetAuthGameMode()) : nullptr)
    {
        GameMode->RestartPlayer(this);
    }
}
