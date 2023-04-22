// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AG_CharacterMovementComponent.generated.h"

class UGameplayAbility;
class UAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class UE5_GAS_MULTIPLAYER_API UAG_CharacterMovementComponent : public UCharacterMovementComponent
{
    GENERATED_BODY()

public:

    bool TryTraversal(UAbilitySystemComponent* AbilitySystemComponent);

protected:

    UPROPERTY(EditDefaultsOnly)
    TArray<TSubclassOf<UGameplayAbility>> TraversalAbilitiesOrdered; 
};
