// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstances/AG_AnimInstance.h"

#include "ue5_gas_multiplayerCharacter.h"
#include "DataAssets/CharacterAnimDataAsset.h"

UBlendSpace* UAG_AnimInstance::GetLocomotionBlendSpace() const
{
    if (const auto* Character = Cast<Aue5_gas_multiplayerCharacter>(GetOwningActor()))
    {
        const FCharacterData Data = Character->GetCharacterData();
        if (Data.CharacterAnimDataAsset)
        {
            return Data.CharacterAnimDataAsset->CharacterAnimData.MovementBlendSpace;
        }
    }
    return nullptr;
}

UAnimationAsset* UAG_AnimInstance::GetIdleAnimation() const
{
    if (const auto* Character = Cast<Aue5_gas_multiplayerCharacter>(GetOwningActor()))
    {
        const FCharacterData Data = Character->GetCharacterData();
        if (Data.CharacterAnimDataAsset)
        {
            return Data.CharacterAnimDataAsset->CharacterAnimData.IdleAnimationAsset;
        }
    }
    return nullptr;
}
