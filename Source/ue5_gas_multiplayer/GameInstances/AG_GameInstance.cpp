// Fill out your copyright notice in the Description page of Project Settings.


#include "AG_GameInstance.h"

#include "AbilitySystemGlobals.h"

void UAG_GameInstance::Init()
{
    Super::Init();

    UAbilitySystemGlobals::Get().InitGlobalData();
}
