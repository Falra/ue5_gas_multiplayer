// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystemVolume.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

AAbilitySystemVolume::AAbilitySystemVolume()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AAbilitySystemVolume::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AAbilitySystemVolume::ActorEnteredVolume(AActor* Other)
{
    Super::ActorEnteredVolume(Other);

    if (!HasAuthority())
    {
        return;
    }

    if (auto* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Other))
    {
        for (const auto Ability : PermanentAbilitiesToGive)
        {
            ASC->GiveAbility(FGameplayAbilitySpec(Ability));
        }
        
        EnteredActorsInfoMap.Add(Other);

        for (const auto Ability : OngoingAbilitiesToGive)
        {
            FGameplayAbilitySpecHandle AbilityHandle = ASC->GiveAbility(FGameplayAbilitySpec(Ability));

            EnteredActorsInfoMap[Other].AppliedAbilities.Add(AbilityHandle);
        }

        FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
        EffectContext.AddInstigator(Other, Other);
        
        for (const auto Effect : OngoingEffectsToApply)
        {
            FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(Effect, 1, EffectContext);

            if (!SpecHandle.IsValid())
            {
                continue;
            }
            
            FActiveGameplayEffectHandle GEHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
            if (GEHandle.WasSuccessfullyApplied())
            {
                EnteredActorsInfoMap[Other].AppliedEffects.Add(GEHandle);
            }
        }

        for (const auto EventTag : GameplayEventsToSendOnEnter)
        {
            FGameplayEventData EventData;
            EventData.EventTag = EventTag;

            UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Other, EventTag, EventData);
        }
    }
}

void AAbilitySystemVolume::ActorLeavingVolume(AActor* Other)
{
    Super::ActorLeavingVolume(Other);
    
    if (!HasAuthority() || !EnteredActorsInfoMap.Contains(Other))
    {
        return;
    }
    if (auto* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Other))
    {
        if (EnteredActorsInfoMap.Contains(Other))
        {
            for (const auto GameplayEffectHandle : EnteredActorsInfoMap[Other].AppliedEffects)
            {
                ASC->RemoveActiveGameplayEffect(GameplayEffectHandle);
            }

            for (const auto GameplayAbilityHandle : EnteredActorsInfoMap[Other].AppliedAbilities)
            {
                ASC->ClearAbility(GameplayAbilityHandle);
            }

            EnteredActorsInfoMap.Remove(Other);
        }

        FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
        EffectContext.AddInstigator(Other, Other);
        
        for (const auto Effect : OnExitEffectsToApply)
        {
            FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(Effect, 1, EffectContext);

            if (!SpecHandle.IsValid())
            {
                continue;
            }
            
            ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
        }
        
        for (const auto EventTag : GameplayEventsToSendOnExit)
        {
            FGameplayEventData EventData;
            EventData.EventTag = EventTag;

            UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Other, EventTag, EventData);
        }
    }
}
