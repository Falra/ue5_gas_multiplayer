// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryItemInstance.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "ActionGameStatics.h"
#include "Actors/ItemActor.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

void UInventoryItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UInventoryItemInstance, ItemStaticDataClass);
    DOREPLIFETIME(UInventoryItemInstance, bEquipped);
    DOREPLIFETIME(UInventoryItemInstance, ItemActor);
}

void UInventoryItemInstance::Init(TSubclassOf<UItemStaticData> InItemStaticDataClass)
{
    ItemStaticDataClass = InItemStaticDataClass;
}

const UItemStaticData* UInventoryItemInstance::GetItemStaticData() const
{
    return UActionGameStatics::GetItemStaticData(ItemStaticDataClass);
}

AItemActor* UInventoryItemInstance::GetItemActor() const
{
    return ItemActor;
}

void UInventoryItemInstance::OnRep_Equipped()
{

}

void UInventoryItemInstance::OnEquipped(AActor* ItemOwner /* = nullptr */)
{
    if (!ItemOwner)
    {
        return;
    }
    
    UWorld* World = ItemOwner->GetWorld();
    if (!World)
    {
        return;
    }
    FTransform Transform;
    const UItemStaticData* ItemStaticData = GetItemStaticData();
    ItemActor = World->SpawnActorDeferred<AItemActor>(ItemStaticData->ItemActorClass, Transform, ItemOwner);
    ItemActor->Init(this);
    ItemActor->OnEquipped();
    ItemActor->FinishSpawning(Transform);
    if (const auto* Character = Cast<ACharacter>(ItemOwner))
    {
        USkeletalMeshComponent* SkeletalMesh = Character->GetMesh();
        ItemActor->AttachToComponent(SkeletalMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, ItemStaticData->AttachmentSocket);
    }
    
    TryGrantAbilities(ItemOwner);
    
    bEquipped = true;
}

void UInventoryItemInstance::OnUnequipped(AActor* ItemOwner /* = nullptr */)
{
    if (IsValid(ItemActor))
    {
        ItemActor->Destroy();
        ItemActor = nullptr;
    }
    
    TryRemoveAbilities(ItemOwner);
    
    bEquipped = false;
}

void UInventoryItemInstance::OnDropped(AActor* ItemOwner /* = nullptr */)
{
    if (IsValid(ItemActor))
    {
        ItemActor->OnDropped();
    }
    
    TryRemoveAbilities(ItemOwner);
    
    bEquipped = false;
}

void UInventoryItemInstance::TryGrantAbilities(AActor* ItemOwner)
{
    if (!ItemOwner || !ItemOwner->HasAuthority())
    {
        return;
    }
    if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(ItemOwner))
    {
        const UItemStaticData* ItemStaticData = GetItemStaticData();
        for (auto& Ability : ItemStaticData->GrantedAbilities)
        {
            GrantedAbilityHandles.Add(ASC->GiveAbility(FGameplayAbilitySpec(Ability)));
        }
    }
}

void UInventoryItemInstance::TryRemoveAbilities(AActor* ItemOwner)
{
    if (!ItemOwner || !ItemOwner->HasAuthority())
    {
        return;
    }
    if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(ItemOwner))
    {
        const UItemStaticData* ItemStaticData = GetItemStaticData();
        for (auto& Ability : GrantedAbilityHandles)
        {
            ASC->ClearAbility(Ability);
        }
        GrantedAbilityHandles.Empty();
    }
}
