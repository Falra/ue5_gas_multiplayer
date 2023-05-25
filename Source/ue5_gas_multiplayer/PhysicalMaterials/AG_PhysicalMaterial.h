// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "AG_PhysicalMaterial.generated.h"

class UNiagaraSystem;

UCLASS()
class UE5_GAS_MULTIPLAYER_API UAG_PhysicalMaterial : public UPhysicalMaterial
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhysicalMaterial")
    USoundBase* FootstepSound = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhysicalMaterial")
    USoundBase* PointImpactSound = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhysicalMaterial")
    UNiagaraSystem* PointImpactEffect = nullptr;
};
