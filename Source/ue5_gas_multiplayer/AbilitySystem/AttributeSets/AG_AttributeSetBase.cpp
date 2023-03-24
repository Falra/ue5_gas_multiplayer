// Fill out your copyright notice in the Description page of Project Settings.


#include "AG_AttributeSetBase.h"

void UAG_AttributeSetBase::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);
}

void UAG_AttributeSetBase::OnRep_Health()
{
}

void UAG_AttributeSetBase::OnRep_MaxHealth()
{
}
