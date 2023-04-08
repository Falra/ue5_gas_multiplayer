// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionGameTypes.h"
#include "ActorComponents/AG_FootstepComponent.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AG_AnimNotify_Step.generated.h"

/**
 * 
 */
UCLASS()
class UE5_GAS_MULTIPLAYER_API UAG_AnimNotify_Step : public UAnimNotify
{
    GENERATED_BODY()
public:
    virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

    UPROPERTY(EditAnywhere)
    EFoot Foot = EFoot::Left;
};
