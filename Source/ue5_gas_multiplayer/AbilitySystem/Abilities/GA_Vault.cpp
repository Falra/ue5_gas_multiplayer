// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GA_Vault.h"

#include "AG_Character.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "ActorComponents/AG_MotionWarpingComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"

UGA_Vault::UGA_Vault()
{
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UGA_Vault::CommitCheck(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, FGameplayTagContainer* OptionalRelevantTags)
{
    if (!Super::CommitCheck(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags))
    {
        return false;
    }
    auto* Character = GetActionGameCharacterFromActorInfo();
    if (!IsValid(Character))
    {
        return false;
    }
    const FVector StartLocation = Character->GetActorLocation();
    const FVector ForwardVector = Character->GetActorForwardVector();
    const FVector UpVector = Character->GetActorUpVector();
    
    TArray<AActor*> ActorsToIgnore {Character};

    static const auto* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("AbilitySystem.ShowDebugTraversal"));
    const bool bShowTraversal = CVar->GetInt() > 0;

    EDrawDebugTrace::Type DrawDebugType = bShowTraversal ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

    bool bJumpToLocationSet = false;
    int32 JumpToLocationIndex = INDEX_NONE;
    uint8 TraceIndex = 0;
    FHitResult TraceHit;
    float MaxJumpDistance = HorizontalTraceLength;
    for (; TraceIndex < HorizontalTraceCount; ++TraceIndex)
    {
        const FVector TraceStart = StartLocation + TraceIndex * UpVector * HorizontalTraceStep;
        const FVector TraceEnd = TraceStart + ForwardVector * HorizontalTraceLength;

        if (UKismetSystemLibrary::SphereTraceSingleForObjects(this, TraceStart, TraceEnd, HorizontalTraceRadius,
            TraceObjectTypes, true, ActorsToIgnore, DrawDebugType, TraceHit, true))
        {
            if (JumpToLocationIndex == INDEX_NONE && (TraceIndex < HorizontalTraceCount - 1))
            {
                JumpToLocationIndex = TraceIndex;
                JumpToLocation = TraceHit.Location;
            }
            else if (JumpToLocationIndex == (TraceIndex - 1))
            {
                MaxJumpDistance = FVector::Dist2D(TraceHit.Location, TraceStart);
                break;
            }
        }
        else
        {
            if (JumpToLocationIndex != INDEX_NONE)
            {
                break;
            }
        }
    }
    if (JumpToLocationIndex == INDEX_NONE)
    {
        return false;
    }
    const float DistanceToJumpTo = FVector::Dist2D(StartLocation, JumpToLocation);
    const float MaxVerticalTraceDistance = MaxJumpDistance - DistanceToJumpTo;
    if (MaxVerticalTraceDistance < 0.f)
    {
        return false;
    }
    if (TraceIndex == HorizontalTraceCount)
    {
        TraceIndex = HorizontalTraceCount - 1;
    }
    const float VerticalTraceLength = FMath::Abs(JumpToLocation.Z - (StartLocation + TraceIndex * UpVector * HorizontalTraceStep).Z);
    FVector VerticalStartLocation = JumpToLocation + UpVector * VerticalTraceLength;
    TraceIndex = 0;
    const float VerticalTraceCount = MaxVerticalTraceDistance / VerticalTraceStep;
    bool bJumpOverLocationSet = false;
    for (; TraceIndex <= VerticalTraceCount; ++TraceIndex)
    {
        const FVector TraceStart = VerticalStartLocation + TraceIndex * ForwardVector * VerticalTraceStep;
        const FVector TraceEnd = TraceStart + UpVector * VerticalTraceLength * -1.f;
        if (UKismetSystemLibrary::SphereTraceSingleForObjects(this, TraceStart, TraceEnd, VerticalTraceRadius,
            TraceObjectTypes, true, ActorsToIgnore, DrawDebugType, TraceHit, true))
        {
            JumpOverLocation = TraceHit.ImpactPoint;
            if (TraceIndex == 0)
            {
                JumpToLocation = JumpOverLocation;
            }
        }
        else if (TraceIndex != 0)
        {
            bJumpOverLocationSet = true;
            break;
        }
    }
    if (!bJumpOverLocationSet)
    {
        return false;
    }

    const FVector TraceStart = JumpOverLocation + ForwardVector * VerticalTraceStep;
    if (UKismetSystemLibrary::SphereTraceSingleForObjects(this, TraceStart, JumpOverLocation, VerticalTraceRadius,
            TraceObjectTypes, true, ActorsToIgnore, DrawDebugType, TraceHit, true))
    {
        JumpOverLocation = TraceHit.ImpactPoint;
    }
    if (bShowTraversal)
    {
        DrawDebugSphere(GetWorld(), JumpToLocation, 15.0f, 16, FColor::White, false, 7.0f);
        DrawDebugSphere(GetWorld(), JumpOverLocation, 15.0f, 16, FColor::White, false, 7.0f);
    }
    
    return true;
}

void UGA_Vault::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
    
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        K2_EndAbility();
        return;
    }
    const auto* Character = GetActionGameCharacterFromActorInfo();
    if (UCharacterMovementComponent* CharacterMovement = Character ? Character->GetCharacterMovement() : nullptr)
    {
        CharacterMovement->SetMovementMode(MOVE_Flying);
    }

    if (UCapsuleComponent* CapsuleComponent = Character ? Character->GetCapsuleComponent() : nullptr)
    {
        for (const ECollisionChannel CollisionChannel : CollisionChannelsToIgnore)
        {
            CapsuleComponent->SetCollisionResponseToChannel(CollisionChannel, ECollisionResponse::ECR_Ignore);
        }
    }

    if (UAG_MotionWarpingComponent* MotionWarping = Character ? Character->GetMotionWarpingComponent() : nullptr)
    {
        MotionWarping->AddOrUpdateWarpTargetFromLocationAndRotation(TEXT("JumpToLocation"), JumpToLocation, Character->GetActorRotation());
        MotionWarping->AddOrUpdateWarpTargetFromLocationAndRotation(TEXT("JumpOverLocation"), JumpOverLocation, Character->GetActorRotation());
    }
    VaultMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, VaultMontage);
    VaultMontageTask->OnBlendOut.AddDynamic(this, &UGA_Vault::K2_EndAbility);
    VaultMontageTask->OnInterrupted.AddDynamic(this, &UGA_Vault::K2_EndAbility);
    VaultMontageTask->OnCancelled.AddDynamic(this, &UGA_Vault::K2_EndAbility);
    VaultMontageTask->OnCompleted.AddDynamic(this, &UGA_Vault::K2_EndAbility);
    VaultMontageTask->ReadyForActivation();
}

void UGA_Vault::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    if (IsValid(VaultMontageTask))
    {
        VaultMontageTask->EndTask();
        VaultMontageTask = nullptr;
    }

    const auto* Character = GetActionGameCharacterFromActorInfo();

    if (UCharacterMovementComponent* CharacterMovement = Character ? Character->GetCharacterMovement() : nullptr;
        CharacterMovement && CharacterMovement->IsFlying())
    {
        CharacterMovement->SetMovementMode(MOVE_Falling);
    }
    
    if (UCapsuleComponent* CapsuleComponent = Character ? Character->GetCapsuleComponent() : nullptr)
    {
        for (const ECollisionChannel CollisionChannel : CollisionChannelsToIgnore)
        {
            CapsuleComponent->SetCollisionResponseToChannel(CollisionChannel, ECollisionResponse::ECR_Block);
        }
    }

    if (UAG_MotionWarpingComponent* MotionWarping = Character ? Character->GetMotionWarpingComponent() : nullptr)
    {
        MotionWarping->RemoveWarpTarget(TEXT("JumpToLocation"));
        MotionWarping->RemoveWarpTarget(TEXT("JumpOverLocation"));
    }
    
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
