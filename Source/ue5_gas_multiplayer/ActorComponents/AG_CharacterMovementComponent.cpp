// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponents/AG_CharacterMovementComponent.h"

#include "AbilitySystemComponent.h"

bool UAG_CharacterMovementComponent::TryTraversal(UAbilitySystemComponent* AbilitySystemComponent)
{
    for (const auto AbilityClass : TraversalAbilitiesOrdered)
    {
        if (AbilitySystemComponent->TryActivateAbilityByClass(AbilityClass, true))
        {
            const FGameplayAbilitySpec* Spec = AbilitySystemComponent->FindAbilitySpecFromClass(AbilityClass);
            if (Spec && Spec->IsActive())
            {
                return true;
            }
        }
    }
    return false;
}
