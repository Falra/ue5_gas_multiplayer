// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_InventoryCombatAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "ActionGameTypes.h"
#include "AG_Character.h"
#include "Camera/CameraComponent.h"
#include "Inventory/ItemActors/WeaponItemActor.h"
#include "Kismet/KismetSystemLibrary.h"

FGameplayEffectSpecHandle UGA_InventoryCombatAbility::GetWeaponEffectSpec(const FHitResult& Hit)
{
    if (const auto* ASC = GetAbilitySystemComponentFromActorInfo())
    {
        if (const UWeaponStaticData* WeaponStaticData = GetEquippedItemWeaponStaticData())
        {
            const FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
            
            FGameplayEffectSpecHandle OutSpec = ASC->MakeOutgoingSpec(WeaponStaticData->DamageEffect, 1, EffectContext);

            UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(OutSpec,
                FGameplayTag::RequestGameplayTag(TEXT("Attribute.Health")), -WeaponStaticData->BaseDamage);

            return OutSpec;
        }
    }
    
    return {};
}

const bool UGA_InventoryCombatAbility::GetWeaponToFocusTraceResult(float TraceDistance, ETraceTypeQuery TraceType,
    FHitResult& OutHitResult) const
{
    AWeaponItemActor* WeaponActor = GetEquippedWeaponItemActor();
    AAG_Character* Character = GetActionGameCharacterFromActorInfo();
    if (!Character || !WeaponActor)
    {
        return false;
    }
    const FTransform& CameraTransform = Character->GetFollowCamera()->GetComponentTransform();
    const FVector FocusTraceEnd = CameraTransform.GetLocation() + CameraTransform.GetRotation().Vector() * TraceDistance;
    const TArray<AActor*> ActorsToIgnore {GetAvatarActorFromActorInfo()};
 
    FHitResult FocusHit;
    UKismetSystemLibrary::LineTraceSingle(GetWorld(), CameraTransform.GetLocation(), FocusTraceEnd, TraceType, false,
        ActorsToIgnore, EDrawDebugTrace::None, FocusHit, true);
    FVector MuzzleLocation = WeaponActor->GetMuzzleLocation();

    const FVector WeaponTraceEnd = MuzzleLocation + (FocusHit.Location - MuzzleLocation).GetSafeNormal() * TraceDistance;
    UKismetSystemLibrary::LineTraceSingle(GetWorld(), MuzzleLocation, WeaponTraceEnd, TraceType, false,
        ActorsToIgnore, EDrawDebugTrace::None, OutHitResult, true);

    return OutHitResult.bBlockingHit;
}
