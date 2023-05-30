// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionGameStatics.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Kismet/KismetSystemLibrary.h"

static TAutoConsoleVariable<int32> CVarShowRadialDamage(
    TEXT("ShowDebugRadialDamage"),
    0,
    TEXT("Draw debug for radial damage.")
    TEXT("0: off\n")
    TEXT("1: on\n"),
    ECVF_Cheat
    );

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
    TArray<AActor*> OutActors;
    const TArray<AActor*> ActorsToIgnore {DamageCauser};
    const bool bDebug = CVarShowRadialDamage.GetValueOnAnyThread() > 0;
    
    UKismetSystemLibrary::SphereOverlapActors(WorldContextObject, Location, Radius, ObjectTypes, nullptr, ActorsToIgnore, OutActors);
    for (AActor* Actor : OutActors)
    {
        FHitResult HitResult;
        if (UKismetSystemLibrary::LineTraceSingle(WorldContextObject, Location, Actor->GetActorLocation(), TraceType,
            true, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true))
        {
            AActor* Target = HitResult.GetActor();
            auto* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);
            if (!Target || Target != Actor || !ASC)
            {
                if (bDebug)
                {
                    DrawDebugLine(WorldContextObject->GetWorld(), Location, Actor->GetActorLocation(),
                        FColor::Red, false, 4.0f, 0, 1.0f);

                    DrawDebugSphere(WorldContextObject->GetWorld(), HitResult.Location, 16, 16,
                        FColor::Red, false, 4.0f, 0, 1.0f);

                    DrawDebugString(WorldContextObject->GetWorld(), HitResult.Location, *GetNameSafe(Target), nullptr,
                        FColor::White, 0, false, 1.0f);
                }
                
                continue;
            }
            bool bWasApplied = false;
            FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
            EffectContext.AddInstigator(DamageCauser, DamageCauser);

            for (auto DamageEffect : DamageEffects)
            {
                FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(DamageEffect, 1, EffectContext);
                if (!SpecHandle.IsValid())
                {
                    continue;
                }
                UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle,
                    FGameplayTag::RequestGameplayTag(TEXT("Attribute.Health")), -DamageAmount);

                FActiveGameplayEffectHandle GEHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
                if (GEHandle.WasSuccessfullyApplied())
                {
                    bWasApplied = true;
                }
            }

            if (bDebug)
            {
                DrawDebugLine(WorldContextObject->GetWorld(), Location, Actor->GetActorLocation(),
                    bWasApplied ? FColor::Green : FColor::Red, false, 4.0f, 0, 1.0f);

                DrawDebugSphere(WorldContextObject->GetWorld(), HitResult.Location, 16, 16,
                    bWasApplied ? FColor::Green : FColor::Red, false, 4.0f, 0, 1.0f);

                DrawDebugString(WorldContextObject->GetWorld(), HitResult.Location, *GetNameSafe(Target), nullptr,
                    FColor::White, 0, false, 1.0f);
            }
        }
        else
        {
            if (bDebug)
            {
                DrawDebugLine(WorldContextObject->GetWorld(), Location, Actor->GetActorLocation(),
                    FColor::Red, false, 4.0f, 0, 1.0f);

                DrawDebugSphere(WorldContextObject->GetWorld(), HitResult.Location, 16, 16,
                    FColor::Red, false, 4.0f, 0, 1.0f);

                DrawDebugString(WorldContextObject->GetWorld(), HitResult.Location, *GetNameSafe(HitResult.GetActor()), nullptr,
                    FColor::White, 0, false, 1.0f);
            }
        }
    }
    if (bDebug)
    {
        DrawDebugSphere(WorldContextObject->GetWorld(), Location, Radius, 16,
                    FColor::Purple, false, 4.0f, 0, 1.0f);
    }
}

AProjectile* UActionGameStatics::LaunchProjectile(UObject* WorldContextObject, TSubclassOf<UProjectileStaticData> ProjectileDataClass,
    FTransform Transform, AActor* Owner, AActor* Instigator)
{
    return nullptr;
}
