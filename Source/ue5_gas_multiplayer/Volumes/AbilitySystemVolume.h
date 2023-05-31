// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PhysicsVolume.h"
#include "AbilitySystemVolume.generated.h"

class UGameplayEffect;
class UGameplayAbility;

USTRUCT(BlueprintType)
struct FAbilityVolumeEnteredActorInfo
{
    GENERATED_BODY()

    TArray<FGameplayAbilitySpecHandle> AppliedAbilities;

    TArray<FActiveGameplayEffectHandle> AppliedEffects;
};

UCLASS()
class UE5_GAS_MULTIPLAYER_API AAbilitySystemVolume : public APhysicsVolume
{
    GENERATED_BODY()

public:

    AAbilitySystemVolume();

    virtual void Tick(float DeltaSeconds) override;

    virtual void ActorEnteredVolume(AActor* Other) override;

    virtual void ActorLeavingVolume(AActor* Other) override;
    
protected:
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<TSubclassOf<UGameplayEffect>> OngoingEffectsToApply;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<TSubclassOf<UGameplayEffect>> OnExitEffectsToApply;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bDrawDebug = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FGameplayTag> GameplayEventsToSendOnEnter;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FGameplayTag> GameplayEventsToSendOnExit;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<TSubclassOf<UGameplayAbility>> OngoingAbilitiesToGive;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<TSubclassOf<UGameplayAbility>> PermanentAbilitiesToGive;

    UPROPERTY(Transient)
    TMap<AActor*, FAbilityVolumeEnteredActorInfo> EnteredActorsInfoMap;
};
