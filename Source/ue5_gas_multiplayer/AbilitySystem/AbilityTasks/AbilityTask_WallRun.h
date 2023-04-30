// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_WallRun.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWallRunWallSideDeterminedDelegate, bool, bLeftSide);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWallRunFinishedDelegate);

class ACharacter;
class UCharacterMovementComponent;

UCLASS()
class UE5_GAS_MULTIPLAYER_API UAbilityTask_WallRun : public UAbilityTask
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable)
    FOnWallRunFinishedDelegate OnWallRunFinished;

    UPROPERTY(BlueprintAssignable)
    FOnWallRunWallSideDeterminedDelegate OnWallRunWallSideDetermined;

    UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility"))
    static UAbilityTask_WallRun* CreateWallRunTask(UGameplayAbility* OwningAbility, ACharacter* InCharacterOwner,
        UCharacterMovementComponent* InCharacterMovement, TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes);

    virtual void Activate() override;

    virtual void OnDestroy(bool bInOwnerFinished) override;

    virtual void TickTask(float DeltaTime) override;

protected:
    
};
