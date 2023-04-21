// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AG_AnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class UE5_GAS_MULTIPLAYER_API UAG_AnimInstance : public UAnimInstance
{
    GENERATED_BODY()

protected:
    UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
    UBlendSpace* GetLocomotionBlendSpace() const;

    UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
    UBlendSpace* GetCrouchLocomotionBlendSpace() const;

    UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
    UAnimSequenceBase* GetIdleAnimation() const;

    UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
    UAnimSequenceBase* GetCrouchIdleAnimation() const;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
    class UCharacterAnimDataAsset* DefaultCharacterAnimDataAsset;

private:
    UCharacterAnimDataAsset* GetCharacterAnimDataAsset() const;
};
