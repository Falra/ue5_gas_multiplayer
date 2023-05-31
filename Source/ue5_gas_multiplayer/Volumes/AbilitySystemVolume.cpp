// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystemVolume.h"

AAbilitySystemVolume::AAbilitySystemVolume()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AAbilitySystemVolume::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AAbilitySystemVolume::ActorEnteredVolume(AActor* Other)
{
    Super::ActorEnteredVolume(Other);
}

void AAbilitySystemVolume::ActorLeavingVolume(AActor* Other)
{
    Super::ActorLeavingVolume(Other);
}
