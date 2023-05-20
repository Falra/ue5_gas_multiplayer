// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AG_AnimNotify_GameplayEvent.generated.h"

UCLASS()
class UE5_GAS_MULTIPLAYER_API UAG_AnimNotify_GameplayEvent : public UAnimNotify
{
    GENERATED_BODY()

public:

    virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
    
    UPROPERTY(EditAnywhere)
    FGameplayEventData Payload;
};
