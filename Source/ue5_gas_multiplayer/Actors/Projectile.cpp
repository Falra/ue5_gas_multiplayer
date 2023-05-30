// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Net/UnrealNetwork.h"

AProjectile::AProjectile()
{
    bReplicates = true;
    SetReplicateMovement(true);

    ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
    ProjectileMovementComponent->Velocity = FVector::ZeroVector;
    ProjectileMovementComponent->OnProjectileStop.AddDynamic(this, &AProjectile::OnProjectileStop);

    ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
    ProjectileMesh->SetupAttachment(GetRootComponent());
    ProjectileMesh->SetIsReplicated(true);
    ProjectileMesh->SetCollisionProfileName(TEXT("Projectile"));
    ProjectileMesh->bReceivesDecals = false;
}

const UProjectileStaticData* AProjectile::GetProjectileStaticData() const
{
    if (IsValid(ProjectileDataClass))
    {
        return GetDefault<UProjectileStaticData>(ProjectileDataClass);
    }
    return nullptr;
}

void AProjectile::BeginPlay()
{
    Super::BeginPlay();
    
    const UProjectileStaticData* ProjectileData = GetProjectileStaticData();
    if (ProjectileData && ProjectileMovementComponent)
    {
        if (ProjectileData->ProjectileMesh)
        {
            ProjectileMesh->SetStaticMesh(ProjectileData->ProjectileMesh);
        }
        ProjectileMovementComponent->bInitialVelocityInLocalSpace = false;
        ProjectileMovementComponent->bRotationFollowsVelocity = true;
        ProjectileMovementComponent->bShouldBounce = false;
        ProjectileMovementComponent->ProjectileGravityScale = ProjectileData->GravityMultiplier;
        ProjectileMovementComponent->InitialSpeed = ProjectileData->InitialSpeed;
        ProjectileMovementComponent->MaxSpeed = ProjectileData->MaxSpeed;
        ProjectileMovementComponent->Bounciness = 0.0f;
        ProjectileMovementComponent->Velocity = ProjectileData->InitialSpeed * GetActorForwardVector();
    }
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