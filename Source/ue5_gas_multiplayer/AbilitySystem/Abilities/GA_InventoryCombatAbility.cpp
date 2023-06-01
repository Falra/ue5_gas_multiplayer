// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_InventoryCombatAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "ActionGameTypes.h"
#include "AG_Character.h"
#include "ActorComponents/AG_InventoryComponent.h"
#include "Camera/CameraComponent.h"
#include "Inventory/ItemActors/WeaponItemActor.h"
#include "Kismet/KismetSystemLibrary.h"

bool UGA_InventoryCombatAbility::CommitAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, FGameplayTagContainer* OptionalRelevantTags)
{
    return Super::CommitAbility(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags) && HasEnoughAmmo();
}

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

bool UGA_InventoryCombatAbility::HasEnoughAmmo() const
{
    if (const UWeaponStaticData* WeaponStaticData = GetEquippedItemWeaponStaticData())
    {
        if (const UAG_InventoryComponent* Inventory = GetInventoryComponent())
        {
            return !WeaponStaticData->AmmoTag.IsValid() || Inventory->GetInventoryTagCount(WeaponStaticData->AmmoTag) > 0;
        }
    }
    return false;
}

void UGA_InventoryCombatAbility::DecAmmo()
{
    if (const UWeaponStaticData* WeaponStaticData = GetEquippedItemWeaponStaticData())
    {
        if (!WeaponStaticData->AmmoTag.IsValid())
        {
            return;
        }
        if (UAG_InventoryComponent* Inventory = GetInventoryComponent())
        {
            Inventory->RemoveItemWithInventoryTag(WeaponStaticData->AmmoTag, 1);
        }
    }
}
