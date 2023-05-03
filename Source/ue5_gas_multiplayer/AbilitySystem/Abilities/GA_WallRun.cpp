// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_WallRun.h"

#include "AbilitySystemComponent.h"
#include "AG_Character.h"
#include "AbilitySystem/AbilityTasks/AbilityTask_WallRun.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UGA_WallRun::UGA_WallRun()
{
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_WallRun::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
    Super::OnGiveAbility(ActorInfo, Spec);

    const auto* Character = Cast<ACharacter>(ActorInfo->AvatarActor);
    if (!Character)
    {
        return;
    }
    auto* CapsuleComponent = Character->GetCapsuleComponent();
    if (!CapsuleComponent)
    {
        return;
    }
    CapsuleComponent->OnComponentHit.AddDynamic(this, &UGA_WallRun::OnCapsuleComponentHit);
}

void UGA_WallRun::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
    if (ActorInfo)
    {
        if (const auto* Character = Cast<ACharacter>(ActorInfo->AvatarActor))
        {
            if (auto* CapsuleComponent = Character->GetCapsuleComponent())
            {
                CapsuleComponent->OnComponentHit.RemoveDynamic(this, &UGA_WallRun::OnCapsuleComponentHit);;
            }
        }
    }
    Super::OnRemoveAbility(ActorInfo, Spec);
}

bool UGA_WallRun::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
    if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
    {
        return false;
    }

    const auto* Character = GetActionGameCharacterFromActorInfo();
    return Character && !Character->GetCharacterMovement()->IsMovingOnGround();
}

void UGA_WallRun::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    auto* Character = GetActionGameCharacterFromActorInfo();
    auto* CharacterMovement = Character ? Character->GetCharacterMovement() : nullptr;
    
    WallRunTask = UAbilityTask_WallRun::CreateWallRunTask(this, Character, CharacterMovement, TraceObjectTypes);
    WallRunTask->OnWallRunFinished.AddDynamic(this, &UGA_WallRun::K2_EndAbility);
    WallRunTask->OnWallRunWallSideDetermined.AddDynamic(this, &UGA_WallRun::OnWallSideDetermined);
    WallRunTask->ReadyForActivation();
}

void UGA_WallRun::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    if (IsValid(WallRunTask))
    {
        WallRunTask->EndTask();
    }
    auto* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->RemoveActiveGameplayEffectBySourceEffect(WallRunLeftSideEffectClass, AbilitySystemComponent);
        AbilitySystemComponent->RemoveActiveGameplayEffectBySourceEffect(WallRunRightSideEffectClass, AbilitySystemComponent);
    }
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_WallRun::OnCapsuleComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
    FVector NormalImpulse, const FHitResult& Hit)
{
    auto* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
    if (!AbilitySystemComponent)
    {
        return;
    }
    AbilitySystemComponent->TryActivateAbility(GetCurrentAbilitySpec()->Handle);
}

void UGA_WallRun::OnWallSideDetermined(bool bLeftSide)
{
    auto* Character = GetActionGameCharacterFromActorInfo();
    const auto* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
    if (!Character || !AbilitySystemComponent)
    {
        return;
    }
    const FGameplayEffectContextHandle EffectContextHandle = AbilitySystemComponent->MakeEffectContext();
    const auto EffectClass = bLeftSide ? WallRunLeftSideEffectClass : WallRunRightSideEffectClass;
    Character->ApplyGameplayEffectToSelf(EffectClass, EffectContextHandle);
}
