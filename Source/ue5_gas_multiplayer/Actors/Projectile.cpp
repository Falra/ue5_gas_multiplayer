// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"

#include "NiagaraFunctionLibrary.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

static TAutoConsoleVariable<int32> CVarShowProjectiles(
    TEXT("ShowDebugProjectiles"),
    0,
    TEXT("Draw debug projectile paths.")
    TEXT("0: off\n")
    TEXT("1: on\n"),
    ECVF_Cheat
    );

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

    if (const int32 ShowDebugProjectiles = CVarShowProjectiles.GetValueOnGameThread())
    {
        DebugDrawPath();
    }
}

void AProjectile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (const UProjectileStaticData* ProjectileData = GetProjectileStaticData())
    {
        UGameplayStatics::PlaySoundAtLocation(this, ProjectileData->ImpactSound, GetActorLocation());
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ProjectileData->ImpactEffect, GetActorLocation());
    }
    
    Super::EndPlay(EndPlayReason);
}

void AProjectile::DebugDrawPath()
{
    if (const UProjectileStaticData* ProjectileData = GetProjectileStaticData())
    {
        FPredictProjectilePathParams PathParams;
        PathParams.StartLocation = GetActorLocation();
        PathParams.LaunchVelocity = ProjectileData->InitialSpeed * GetActorForwardVector();
        PathParams.OverrideGravityZ = ProjectileData->GravityMultiplier;
        PathParams.TraceChannel = ECollisionChannel::ECC_Visibility;
        PathParams.bTraceComplex = true;
        PathParams.bTraceWithCollision = true;
        PathParams.DrawDebugType = EDrawDebugTrace::ForDuration;
        PathParams.DrawDebugTime = 3.0f;

        FPredictProjectilePathResult PathResult;
        if (UGameplayStatics::PredictProjectilePath(this, PathParams, PathResult))
        {
            DrawDebugSphere(GetWorld(), PathResult.HitResult.Location, 50.0f, 16, FColor::Red);
        }
    }
}

void AProjectile::OnProjectileStop(const FHitResult& HitResult)
{
    if (const UProjectileStaticData* ProjectileData = GetProjectileStaticData())
    {
        
    }
    
    Destroy();
}

void AProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AProjectile, ProjectileDataClass);
}