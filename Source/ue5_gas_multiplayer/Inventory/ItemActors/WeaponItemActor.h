// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/ItemActor.h"
#include "WeaponItemActor.generated.h"

UCLASS()
class UE5_GAS_MULTIPLAYER_API AWeaponItemActor : public AItemActor
{
    GENERATED_BODY()

public:
    AWeaponItemActor();

    const UWeaponStaticData* GetWeaponStaticData() const;

    UFUNCTION(BlueprintPure, BlueprintCallable)
    FVector GetMuzzleLocation() const;

    UFUNCTION(BlueprintPure, BlueprintCallable)
    void PlayWeaponEffects(const FHitResult& HitResult) const;
    
protected:
    virtual void InitInternal() override;

    UFUNCTION(NetMulticast, Reliable)
    void MulticastPlayWeaponEffects(const FHitResult& HitResult) const;

    void PlayWeaponEffectsInternal(const FHitResult& HitResult) const;
    
    UPROPERTY()
    UMeshComponent* MeshComponent;
};
