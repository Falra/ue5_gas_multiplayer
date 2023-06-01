// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FastArrayTagCounter.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "Inventory/InventoryList.h"
#include "AG_InventoryComponent.generated.h"


struct FGameplayEventData;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class UE5_GAS_MULTIPLAYER_API UAG_InventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAG_InventoryComponent();

    virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override; 

    virtual void InitializeComponent() override;

    UFUNCTION(BlueprintCallable)
    void AddItem(TSubclassOf<UItemStaticData> InItemStaticDataClass);

    UFUNCTION(BlueprintCallable)
    void AddItemInstance(UInventoryItemInstance* InItemInstance);
    
    UFUNCTION(BlueprintCallable)
    void RemoveItem(TSubclassOf<UItemStaticData> InItemStaticDataClass);

    UFUNCTION(BlueprintCallable)
    void RemoveItemInstance(UInventoryItemInstance* InItemInstance);

    UFUNCTION(BlueprintCallable)
    void RemoveItemWithInventoryTag(FGameplayTag Tag, int32 Count = 1);
    
    UFUNCTION(BlueprintCallable)
    void EquipItem(TSubclassOf<UItemStaticData> InItemStaticDataClass);

    UFUNCTION(BlueprintCallable)
    void EquipItemInstance(UInventoryItemInstance* InItemInstance);

    UFUNCTION(BlueprintCallable)
    void UnequipItem();

    UFUNCTION(BlueprintCallable)
    void DropItem();

    UFUNCTION(BlueprintCallable)
    void EquipNextItem();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    UInventoryItemInstance* GetEquippedItem() const { return CurrentItem; }

    virtual void GameplayEventCallback(const FGameplayEventData* Payload);

    static FGameplayTag EquipItemActorTag;
    static FGameplayTag DropItemTag;
    static FGameplayTag EquipNextTag;
    static FGameplayTag UnequipTag;

    UFUNCTION(BlueprintCallable, BlueprintPure)
    int32 GetInventoryTagCount(FGameplayTag Tag) const;

    UFUNCTION(BlueprintCallable)
    void AddInventoryTagCount(FGameplayTag Tag, int32 Count = 1);
    
protected:
    
    UFUNCTION()
    void AddInventoryTags() const;
    
    UPROPERTY(Replicated)
    FInventoryList InventoryList;

    UPROPERTY(EditDefaultsOnly)
    TArray<TSubclassOf<UItemStaticData>> DefaultItems;

    UPROPERTY(Replicated)
    UInventoryItemInstance* CurrentItem;

    UPROPERTY(Replicated)
    FFastArrayTagCounter InventoryTags;

    TArray<UInventoryItemInstance*> GetAllInstancesWithTag(FGameplayTag Tag);
    
    void HandleGameplayEventInternal(const FGameplayEventData& Payload);

    UFUNCTION(Server, Reliable)
    void ServerHandleGameplayEvent(FGameplayEventData Payload);
    
public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

};
