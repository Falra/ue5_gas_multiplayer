// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AG_GameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemLog.h"
#include "AG_Character.h"

void UAG_GameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    const FGameplayEffectContextHandle EffectContext = ActorInfo->AbilitySystemComponent->MakeEffectContext();
    for (auto GameplayEffect : OngoingEffectsToJustApplyOnStart)
    {
        if (!GameplayEffect.Get())
        {
            continue;
        }
        FGameplayEffectSpecHandle SpecHandle = ActorInfo->AbilitySystemComponent->MakeOutgoingSpec(GameplayEffect, 1, EffectContext);
        if (!SpecHandle.IsValid())
        {
            continue;
        }
        FActiveGameplayEffectHandle ActiveGEHandle = ActorInfo->AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
        if (!ActiveGEHandle.WasSuccessfullyApplied())
        {
            ABILITY_LOG(Log, TEXT("Ability %s failed to apply startup effect %s"), *GetName(), *GetNameSafe(GameplayEffect));
        }
    }

    if (IsInstantiated())
    {
        for (auto GameplayEffect : OngoingEffectsToRemoveOnEnd)
        {
            if (!GameplayEffect.Get())
            {
                continue;
            }
            FGameplayEffectSpecHandle SpecHandle = ActorInfo->AbilitySystemComponent->MakeOutgoingSpec(GameplayEffect, 1, EffectContext);
            if (!SpecHandle.IsValid())
            {
                continue;
            }
            FActiveGameplayEffectHandle ActiveGEHandle = ActorInfo->AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
            if (!ActiveGEHandle.WasSuccessfullyApplied())
            {
                ABILITY_LOG(Log, TEXT("Ability %s failed to apply runtime effect %s"), *GetName(), *GetNameSafe(GameplayEffect));
                continue;
            }
            RemoveOnEndEffectHandles.Add(ActiveGEHandle);
        }
    }
}

void UAG_GameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    if (IsInstantiated())
    {
        for (auto ActiveGEHandle: RemoveOnEndEffectHandles)
        {
            if (!ActiveGEHandle.IsValid())
            {
                continue;
            }
            ActorInfo->AbilitySystemComponent->RemoveActiveGameplayEffect(ActiveGEHandle);
        }
    }
    
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

AAG_Character* UAG_GameplayAbility::GetActionGameCharacterFromActorInfo() const
{
    return Cast<AAG_Character>(GetAvatarActorFromActorInfo());
}
