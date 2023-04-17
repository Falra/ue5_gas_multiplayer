// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AG_GameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemLog.h"
#include "AG_Character.h"

void UAG_GameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    if (!ActorInfo->AbilitySystemComponent.IsValid())
    {
        return;
    }
    
    auto* AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get();
    const FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
    for (auto GameplayEffect : OngoingEffectsToJustApplyOnStart)
    {
        if (!GameplayEffect.Get())
        {
            continue;
        }
        FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffect, 1, EffectContext);
        if (!SpecHandle.IsValid())
        {
            continue;
        }
        FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
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
            FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffect, 1, EffectContext);
            if (!SpecHandle.IsValid())
            {
                continue;
            }
            FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
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
    if (!ActorInfo->AbilitySystemComponent.IsValid())
    {
        Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
        return;
    }
    
    if (IsInstantiated())
    {
        auto* AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get();
        
        for (auto ActiveGEHandle: RemoveOnEndEffectHandles)
        {
            if (!ActiveGEHandle.IsValid())
            {
                continue;
            }
            AbilitySystemComponent->RemoveActiveGameplayEffect(ActiveGEHandle);
        }
        RemoveOnEndEffectHandles.Empty();
    }
    
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

AAG_Character* UAG_GameplayAbility::GetActionGameCharacterFromActorInfo() const
{
    return Cast<AAG_Character>(GetAvatarActorFromActorInfo());
}
