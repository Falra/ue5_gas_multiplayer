// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponItemActor.h"

AWeaponItemActor::AWeaponItemActor()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AWeaponItemActor::BeginPlay()
{
    Super::BeginPlay();
}

void AWeaponItemActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}
