// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/AG_AnimNotify_Step.h"

#include "../AG_Character.h"
#include "ActorComponents/AG_FootstepComponent.h"

void UAG_AnimNotify_Step::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    Super::Notify(MeshComp, Animation, EventReference);

    check(MeshComp);
    const auto* Character = Cast<AAG_Character>(MeshComp->GetOwner());
    if (!Character)
    {
        return;
    }
    if (auto* FootstepComponent = Character->GetFootstepComponent())
    {
        FootstepComponent->HandleFootstep(Foot);
    }
}
