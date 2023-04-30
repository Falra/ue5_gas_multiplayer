// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityTask_WallRun.h"

UAbilityTask_WallRun* UAbilityTask_WallRun::CreateWallRunTask(UGameplayAbility* OwningAbility, ACharacter* InCharacterOwner,
    UCharacterMovementComponent* InCharacterMovement, TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes)
{
    return nullptr;
}

void UAbilityTask_WallRun::Activate()
{
    Super::Activate();
}

void UAbilityTask_WallRun::OnDestroy(bool bInOwnerFinished)
{
    Super::OnDestroy(bInOwnerFinished);
}

void UAbilityTask_WallRun::TickTask(float DeltaTime)
{
    Super::TickTask(DeltaTime);
}
