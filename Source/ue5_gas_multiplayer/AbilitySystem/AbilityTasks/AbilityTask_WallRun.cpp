// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityTask_WallRun.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UAbilityTask_WallRun* UAbilityTask_WallRun::CreateWallRunTask(UGameplayAbility* OwningAbility, ACharacter* InCharacterOwner,
    UCharacterMovementComponent* InCharacterMovement, TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes)
{
    auto* WallRunTask = NewAbilityTask<UAbilityTask_WallRun>(OwningAbility);
    WallRunTask->CharacterOwner = InCharacterOwner;
    WallRunTask->CharacterMovement = InCharacterMovement;
    WallRunTask->WallRun_TraceObjectTypes = TraceObjectTypes;
    WallRunTask->bTickingTask = true;
    return WallRunTask;
}

void UAbilityTask_WallRun::Activate()
{
    Super::Activate();

    FHitResult OnWallHit;
    // const FVector CurrentAcceleration = CharacterMovement->GetCurrentAcceleration();

    if (!FindRunnableWall(OnWallHit))
    {
        if (ShouldBroadcastAbilityTaskDelegates())
        {
            OnWallRunFinished.Broadcast();
        }

        EndTask();
        return;
    }

    OnWallRunWallSideDetermined.Broadcast(IsWallOnTheLeftSide(OnWallHit));

    CharacterOwner->Landed(OnWallHit);
    CharacterOwner->SetActorLocation(OnWallHit.ImpactPoint + OnWallHit.ImpactNormal * 60.0f);
    CharacterMovement->SetMovementMode(MOVE_Flying);
}

void UAbilityTask_WallRun::OnDestroy(bool bInOwnerFinished)
{
    Super::OnDestroy(bInOwnerFinished);
}

void UAbilityTask_WallRun::TickTask(float DeltaTime)
{
    Super::TickTask(DeltaTime);
}

bool UAbilityTask_WallRun::FindRunnableWall(FHitResult& OnWallHit)
{
    return false;
}

bool UAbilityTask_WallRun::IsWallOnTheLeftSide(const FHitResult& InWallHit) const
{
    return false;
}
