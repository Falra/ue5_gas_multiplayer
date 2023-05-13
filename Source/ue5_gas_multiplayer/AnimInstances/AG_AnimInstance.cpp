// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstances/AG_AnimInstance.h"

#include "../AG_Character.h"
#include "ActorComponents/AG_InventoryComponent.h"
#include "DataAssets/CharacterAnimDataAsset.h"
#include "Inventory/InventoryItemInstance.h"

#define GET_ITEM_ANIM_DATA(PropertyName) \
{ \
    const UItemStaticData* EquippedItemData = GetEquippedItemData();\
    if (EquippedItemData && EquippedItemData->CharacterAnimData.##PropertyName)\
    {\
        return EquippedItemData->CharacterAnimData.##PropertyName;\
    }\
}

UBlendSpace* UAG_AnimInstance::GetLocomotionBlendSpace() const
{
    GET_ITEM_ANIM_DATA(MovementBlendSpace);
    const auto* MyCharacterAnimDataAsset = GetCharacterAnimDataAsset();
    return MyCharacterAnimDataAsset ? MyCharacterAnimDataAsset->CharacterAnimData.MovementBlendSpace : nullptr;
}

UBlendSpace* UAG_AnimInstance::GetCrouchLocomotionBlendSpace() const
{
    GET_ITEM_ANIM_DATA(CrouchBlendSpace);
    const auto* MyCharacterAnimDataAsset = GetCharacterAnimDataAsset();
    return MyCharacterAnimDataAsset ? MyCharacterAnimDataAsset->CharacterAnimData.CrouchBlendSpace : nullptr;
}

UAnimSequenceBase* UAG_AnimInstance::GetIdleAnimation() const
{
    GET_ITEM_ANIM_DATA(IdleAnimationAsset);
    const auto* MyCharacterAnimDataAsset = GetCharacterAnimDataAsset();
    return MyCharacterAnimDataAsset ? MyCharacterAnimDataAsset->CharacterAnimData.IdleAnimationAsset : nullptr;
}

UAnimSequenceBase* UAG_AnimInstance::GetCrouchIdleAnimation() const
{
    GET_ITEM_ANIM_DATA(CrouchIdleAnimationAsset);
    const auto* MyCharacterAnimDataAsset = GetCharacterAnimDataAsset();
    return MyCharacterAnimDataAsset ? MyCharacterAnimDataAsset->CharacterAnimData.CrouchIdleAnimationAsset : nullptr;
}

const UItemStaticData* UAG_AnimInstance::GetEquippedItemData() const
{
    if (const auto* Character = Cast<AAG_Character>(GetOwningActor()))
    {
        const UAG_InventoryComponent* InventoryComponent = Character->GetInventoryComponent();
        if (const UInventoryItemInstance* ItemInstance = InventoryComponent->GetEquippedItem())
        {
            return ItemInstance->GetItemStaticData();
        }
    }
    return nullptr;
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
