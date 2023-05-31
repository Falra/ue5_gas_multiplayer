// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystemVolume.h"


AAbilitySystemVolume::AAbilitySystemVolume()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AAbilitySystemVolume::BeginPlay()
{
    Super::BeginPlay();
}

void AAbilitySystemVolume::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}
