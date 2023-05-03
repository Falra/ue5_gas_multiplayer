// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityTask_WallRun.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"

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
    CharacterMovement->SetPlaneConstraintEnabled(false);
    CharacterMovement->SetMovementMode(MOVE_Falling);
    
    Super::OnDestroy(bInOwnerFinished);
}

void UAbilityTask_WallRun::TickTask(float DeltaTime)
{
    Super::TickTask(DeltaTime);

    FHitResult OnWallHit;
    
    if (!FindRunnableWall(OnWallHit))
    {
        if (ShouldBroadcastAbilityTaskDelegates())
        {
            OnWallRunFinished.Broadcast();
        }

        EndTask();
        return;
    }

    const FRotator DirectionRotator = IsWallOnTheLeftSide(OnWallHit) ? FRotator(0, -90, 0) : FRotator(0, 90, 0);
    const FVector WallRunDirection = DirectionRotator.RotateVector(OnWallHit.ImpactNormal);
    CharacterMovement->Velocity = WallRunDirection * 700.0f;
    CharacterMovement->Velocity.Z = 0; // CharacterMovement->GetGravityZ() * DeltaTime;
    CharacterOwner->SetActorRotation(WallRunDirection.Rotation());
    CharacterMovement->SetPlaneConstraintEnabled(true);
    CharacterMovement->SetPlaneConstraintOrigin(OnWallHit.ImpactPoint);
    CharacterMovement->SetPlaneConstraintNormal(OnWallHit.ImpactNormal);
}

bool UAbilityTask_WallRun::FindRunnableWall(FHitResult& OnWallHit)
{
    const FVector CharacterLocation = CharacterOwner->GetActorLocation();
    const FVector RightVector = CharacterOwner->GetActorRightVector();
    const FVector ForwardVector = CharacterOwner->GetActorForwardVector();
    const float TraceLength = CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleRadius() + 30.0f;

    const TArray<AActor*> IgnoreActors {CharacterOwner};

    static const auto* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("AbilitySystem.ShowDebugTraversal"));
    const bool bShowTraversal = CVar->GetInt() > 0;

    const EDrawDebugTrace::Type DrawDebugType = bShowTraversal ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

    if (UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), CharacterLocation, CharacterLocation + ForwardVector * TraceLength,
        WallRun_TraceObjectTypes, true, IgnoreActors, DrawDebugType, OnWallHit, true))
    {
        return false;
    }

    if (UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), CharacterLocation, CharacterLocation + -RightVector * TraceLength,
        WallRun_TraceObjectTypes, true, IgnoreActors, DrawDebugType, OnWallHit, true))
    {
        if (FVector::DotProduct(OnWallHit.ImpactNormal, RightVector) > 0.3f)
        {
            return true;
        }
    }

    if (UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), CharacterLocation, CharacterLocation + RightVector * TraceLength,
        WallRun_TraceObjectTypes, true, IgnoreActors, DrawDebugType, OnWallHit, true))
    {
        if (FVector::DotProduct(OnWallHit.ImpactNormal, -RightVector) > 0.3f)
        {
            return true;
        }
    }
    
    return false;
}

bool UAbilityTask_WallRun::IsWallOnTheLeftSide(const FHitResult& InWallHit) const
{
    return FVector::DotProduct(CharacterOwner->GetActorRightVector(), InWallHit.ImpactNormal) > 0.0f;
}
