// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionGameTypes.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class UE5_GAS_MULTIPLAYER_API AProjectile : public AActor
{
    GENERATED_BODY()

public:
    AProjectile();

    const UProjectileStaticData* GetProjectileStaticData() const;

    UPROPERTY(BlueprintReadOnly, Replicated)
    TSubclassOf<UProjectileStaticData> ProjectileDataClass;
    
protected:
    virtual void BeginPlay() override;

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UPROPERTY()
    class UProjectileMovementComponent* ProjectileMovementComponent;

    UPROPERTY()
    class UStaticMeshComponent* ProjectileMesh; 

    void DebugDrawPath();

    UFUNCTION()
    void OnProjectileStop(const FHitResult& HitResult);
    
};
