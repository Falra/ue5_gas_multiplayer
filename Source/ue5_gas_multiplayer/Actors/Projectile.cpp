// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"

#include "Net/UnrealNetwork.h"

AProjectile::AProjectile()
{

}

const UProjectileStaticData* AProjectile::GetProjectileStaticData() const
{
    return nullptr;
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
    Super::BeginPlay();
}

void AProjectile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
}

void AProjectile::DebugDrawPath()
{
}

void AProjectile::OnProjectileStop(const FHitResult& HitResult)
{
}

void AProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AProjectile, ProjectileDataClass);
}