// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstances/AG_AnimInstance.h"

#include "ue5_gas_multiplayerCharacter.h"
#include "DataAssets/CharacterAnimDataAsset.h"

UBlendSpace* UAG_AnimInstance::GetLocomotionBlendSpace() const
{
    const auto* MyCharacterAnimDataAsset = GetCharacterAnimDataAsset();
    return MyCharacterAnimDataAsset ? MyCharacterAnimDataAsset->CharacterAnimData.MovementBlendSpace : nullptr;
}

UAnimSequenceBase* UAG_AnimInstance::GetIdleAnimation() const
{
    const auto* MyCharacterAnimDataAsset = GetCharacterAnimDataAsset();
    return MyCharacterAnimDataAsset ? MyCharacterAnimDataAsset->CharacterAnimData.IdleAnimationAsset : nullptr;
}

UCharacterAnimDataAsset* UAG_AnimInstance::GetCharacterAnimDataAsset() const
{
    if (const auto* Character = Cast<Aue5_gas_multiplayerCharacter>(GetOwningActor()))
    {
        const FCharacterData Data = Character->GetCharacterData();
        if (Data.CharacterAnimDataAsset)
        {
            return Data.CharacterAnimDataAsset;
        }
    }
    return DefaultCharacterAnimDataAsset;
}
