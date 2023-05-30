// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionGameStatics.h"

const UItemStaticData* UActionGameStatics::GetItemStaticData(TSubclassOf<UItemStaticData> ItemDataClass)
{
    if (!IsValid(ItemDataClass))
    {
        return nullptr;
    }
    return ItemDataClass->GetDefaultObject<UItemStaticData>();
}

void UActionGameStatics::ApplyRadialDamage(UObject* WorldContextObject, AActor* DamageCauser, FVector Location, float Radius,
    float DamageAmount, TArray<TSubclassOf<UGameplayEffect>> DamageEffects, const TArray<TEnumAsByte<EObjectTypeQuery>>& ObjectTypes,
    ETraceTypeQuery TraceType)
{
}

AProjectile* UActionGameStatics::LaunchProjectile(UObject* WorldContextObject, TSubclassOf<UProjectileStaticData> ProjectileDataClass,
    FTransform Transform, AActor* Owner, AActor* Instigator)
{
    return nullptr;
}
