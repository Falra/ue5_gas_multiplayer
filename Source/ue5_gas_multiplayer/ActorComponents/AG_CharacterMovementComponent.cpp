// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponents/AG_CharacterMovementComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"

static TAutoConsoleVariable<int32> CVarShowTraversal(
    TEXT("AbilitySystem.ShowDebugTraversal"),
    0,
    TEXT("Draw debug info aabout traversal")
    TEXT("0: off\n")
    TEXT("1: on\n"),
    ECVF_Cheat
    );

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

void UAG_CharacterMovementComponent::BeginPlay()
{
    Super::BeginPlay();
    HandleMovementDirection();
    if (auto* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()))
    {
        ASC->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(TEXT("Movement.Enforced.Strafe")), EGameplayTagEventType::NewOrRemoved).
            AddUObject(this, &UAG_CharacterMovementComponent::OnEnforceStrafeTagChanged);
    }
}

void UAG_CharacterMovementComponent::SetMovementDirectionType(EMovementDirectionType NewMovementDirectionType)
{
    MovementDirectionType = NewMovementDirectionType;
    HandleMovementDirection();
}

void UAG_CharacterMovementComponent::OnEnforceStrafeTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
    if (NewCount)
    {
        SetMovementDirectionType(EMovementDirectionType::Strafe);
    }
    else
    {
        SetMovementDirectionType(EMovementDirectionType::OrientToMovement);
    }
}

void UAG_CharacterMovementComponent::HandleMovementDirection()
{
    switch (MovementDirectionType)
    {
        case EMovementDirectionType::Strafe:
            bUseControllerDesiredRotation = true;
            bOrientRotationToMovement = false;
            CharacterOwner->bUseControllerRotationYaw = true;
            break;
        default:
            bUseControllerDesiredRotation = false;
            bOrientRotationToMovement = true;
            CharacterOwner->bUseControllerRotationYaw = false;
            break;
    }
}
