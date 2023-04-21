// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstances/AG_AnimInstance.h"

#include "../AG_Character.h"
#include "DataAssets/CharacterAnimDataAsset.h"

UBlendSpace* UAG_AnimInstance::GetLocomotionBlendSpace() const
{
    const auto* MyCharacterAnimDataAsset = GetCharacterAnimDataAsset();
    return MyCharacterAnimDataAsset ? MyCharacterAnimDataAsset->CharacterAnimData.MovementBlendSpace : nullptr;
}

UBlendSpace* UAG_AnimInstance::GetCrouchLocomotionBlendSpace() const
{
    const auto* MyCharacterAnimDataAsset = GetCharacterAnimDataAsset();
    return MyCharacterAnimDataAsset ? MyCharacterAnimDataAsset->CharacterAnimData.CrouchBlendSpace : nullptr;
}

UAnimSequenceBase* UAG_AnimInstance::GetIdleAnimation() const
{
    const auto* MyCharacterAnimDataAsset = GetCharacterAnimDataAsset();
    return MyCharacterAnimDataAsset ? MyCharacterAnimDataAsset->CharacterAnimData.IdleAnimationAsset : nullptr;
}

UAnimSequenceBase* UAG_AnimInstance::GetCrouchIdleAnimation() const
{
    const auto* MyCharacterAnimDataAsset = GetCharacterAnimDataAsset();
    return MyCharacterAnimDataAsset ? MyCharacterAnimDataAsset->CharacterAnimData.CrouchIdleAnimationAsset : nullptr;
}

UCharacterAnimDataAsset* UAG_AnimInstance::GetCharacterAnimDataAsset() const
{
    if (const auto* Character = Cast<AAG_Character>(GetOwningActor()))
    {
        const FCharacterData Data = Character->GetCharacterData();
        if (Data.CharacterAnimDataAsset)
        {
            return Data.CharacterAnimDataAsset;
        }
    }
    return DefaultCharacterAnimDataAsset;
}
