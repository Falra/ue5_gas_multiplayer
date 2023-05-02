// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AG_GameplayAbility.h"
#include "GA_WallRun.generated.h"

class UAbilityTask_WallRun;

UCLASS()
class UE5_GAS_MULTIPLAYER_API UGA_WallRun : public UAG_GameplayAbility
{
    GENERATED_BODY()

public:
    UGA_WallRun();

    virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

    virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

    virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags,
        FGameplayTagContainer* OptionalRelevantTags) const override;

    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
    UFUNCTION()
    void OnCapsuleComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse,
        const FHitResult& Hit);

    UFUNCTION()
    void OnWallSideDetermined(bool bLeftSide);

    UPROPERTY()
    UAbilityTask_WallRun* WallRunTask = nullptr;
    
    UPROPERTY(EditDefaultsOnly)
    TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;
    
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UGameplayEffect> WallRunLeftSideEffectClass;
    
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UGameplayEffect> WallRunRightSideEffectClass;
    
};
