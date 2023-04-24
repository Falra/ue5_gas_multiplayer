// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GA_Vault.h"

#include "AG_Character.h"
#include "Kismet/KismetSystemLibrary.h"

UGA_Vault::UGA_Vault()
{
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UGA_Vault::CommitCheck(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, FGameplayTagContainer* OptionalRelevantTags)
{
    if (!Super::CommitCheck(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags))
    {
        return false;
    }
    auto* Character = GetActionGameCharacterFromActorInfo();
    if (!IsValid(Character))
    {
        return false;
    }
    const FVector StartLocation = Character->GetActorLocation();
    const FVector ForwardVector = Character->GetActorForwardVector();
    const FVector UpVector = Character->GetActorUpVector();
    
    TArray<AActor*> ActorsToIgnore {Character};

    static const auto* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("AbilitySystem.ShowDebugTraversal"));
    const bool bShowTraversal = CVar->GetInt() > 0;

    EDrawDebugTrace::Type DrawDebugType = bShowTraversal ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

    bool bJumpToLocationSet = false;
    int32 JumpToLocationIndex = INDEX_NONE;
    uint8 TraceIndex = 0;
    FHitResult TraceHit;
    float MaxJumpDistance = HorizontalTraceLength;
    for (; TraceIndex < HorizontalTraceCount; ++TraceIndex)
    {
        const FVector TraceStart = StartLocation + TraceIndex * UpVector * HorizontalTraceStep;
        const FVector TraceEnd = TraceStart + ForwardVector * HorizontalTraceLength;

        if (UKismetSystemLibrary::SphereTraceSingleForObjects(this, TraceStart, TraceEnd, HorizontalTraceRadius, TraceObjectTypes, true,
            ActorsToIgnore, DrawDebugType, TraceHit, true))
        {
            if (JumpToLocationIndex == INDEX_NONE && (TraceIndex < HorizontalTraceCount - 1))
            {
                JumpToLocationIndex = TraceIndex;
                JumpToLocation = TraceHit.Location;
            }
            else if (JumpToLocationIndex == (TraceIndex - 1))
            {
                MaxJumpDistance = FVector::Dist2D(TraceHit.Location, TraceStart);
                break;
            }
        }
        else
        {
            if (JumpToLocationIndex != INDEX_NONE)
            {
                break;
            }
        }
    }
    if (JumpToLocationIndex == INDEX_NONE)
    {
        return false;
    }
    return true;
}

void UGA_Vault::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UGA_Vault::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
