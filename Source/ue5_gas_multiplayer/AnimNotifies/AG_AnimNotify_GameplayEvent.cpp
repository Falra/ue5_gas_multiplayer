// Fill out your copyright notice in the Description page of Project Settings.


#include "AG_AnimNotify_GameplayEvent.h"

#include "AbilitySystemBlueprintLibrary.h"

void UAG_AnimNotify_GameplayEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
    const FAnimNotifyEventReference& EventReference)
{
    Super::Notify(MeshComp, Animation, EventReference);

    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), Payload.EventTag, Payload);
}
