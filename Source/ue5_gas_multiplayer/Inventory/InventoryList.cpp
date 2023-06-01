// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryList.h"

#include "ActionGameStatics.h"
#include "ActionGameTypes.h"
#include "InventoryItemInstance.h"

void FInventoryList::AddItem(TSubclassOf<UItemStaticData> InItemStaticDataClass)
{
    FInventoryListItem& Item = Items.AddDefaulted_GetRef();
    Item.ItemInstance = NewObject<UInventoryItemInstance>();
    Item.ItemInstance->Init(InItemStaticDataClass, UActionGameStatics::GetItemStaticData(InItemStaticDataClass)->MaxStackCount);
    MarkItemDirty(Item);
}

void FInventoryList::AddItem(UInventoryItemInstance* InItemInstance)
{
    FInventoryListItem& Item = Items.AddDefaulted_GetRef();
    Item.ItemInstance = InItemInstance;
    MarkItemDirty(Item);
}

void FInventoryList::RemoveItem(TSubclassOf<UItemStaticData> InItemStaticDataClass)
{
    for (auto ItemIter = Items.CreateIterator(); ItemIter; ++ItemIter)
    {
        const FInventoryListItem& Item = *ItemIter;
        if (Item.ItemInstance && Item.ItemInstance->GetItemStaticData()->IsA(InItemStaticDataClass))
        {
            ItemIter.RemoveCurrent();
            MarkArrayDirty();
            break;
        }
    }
}

void FInventoryList::RemoveItem(UInventoryItemInstance* InItemInstance)
{
    for (auto ItemIter = Items.CreateIterator(); ItemIter; ++ItemIter)
    {
        const FInventoryListItem& Item = *ItemIter;
        if (Item.ItemInstance && Item.ItemInstance == InItemInstance)
        {
            ItemIter.RemoveCurrent();
            MarkArrayDirty();
            break;
        }
    }
}

TArray<UInventoryItemInstance*> FInventoryList::GetAllInstancesWithTag(FGameplayTag Tag)
{
    TArray<UInventoryItemInstance*> OutInstances;
    for (auto ItemIter = Items.CreateIterator(); ItemIter; ++ItemIter)
    {
        FInventoryListItem& Item = *ItemIter;
        if (Item.ItemInstance && Item.ItemInstance->GetItemStaticData()->InventoryTags.Contains(Tag))
        {
            OutInstances.Add(Item.ItemInstance);
        }
    }
    return OutInstances;
}

TArray<UInventoryItemInstance*> FInventoryList::GetAllAvailableInstancesOfType(TSubclassOf<UItemStaticData> InItemStaticDataClass)
{
    TArray<UInventoryItemInstance*> OutInstances;
    for (auto ItemIter = Items.CreateIterator(); ItemIter; ++ItemIter)
    {
        FInventoryListItem& Item = *ItemIter;
        if (!Item.ItemInstance)
        {
            continue;
        }
        const UItemStaticData* ItemData = Item.ItemInstance->GetItemStaticData();
        const bool bSameType = ItemData->IsA(InItemStaticDataClass);
        const bool bHasCapacity = ItemData->MaxStackCount > Item.ItemInstance->GetQuantity();
        if (bSameType && bHasCapacity)
        {
            OutInstances.Add(Item.ItemInstance);
        }
    }
    return OutInstances;
}
