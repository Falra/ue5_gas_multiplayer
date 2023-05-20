// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ActionGameTypes.h"
#include "GameplayTagContainer.h"
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

    virtual void BeginPlay() override;

    UFUNCTION(BlueprintPure, BlueprintCallable)
    EMovementDirectionType GetMovementDirectionType() const { return MovementDirectionType; }

    UFUNCTION(BlueprintCallable)
    void SetMovementDirectionType(EMovementDirectionType NewMovementDirectionType);

    UFUNCTION()
    void OnEnforceStrafeTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
    
protected:

    UPROPERTY(EditDefaultsOnly)
    TArray<TSubclassOf<UGameplayAbility>> TraversalAbilitiesOrdered;

    UPROPERTY(EditAnywhere)
    EMovementDirectionType MovementDirectionType;

    void HandleMovementDirection();
};
