// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryList.h"

#include "ActionGameTypes.h"
#include "InventoryItemInstance.h"

void FInventoryList::AddItem(TSubclassOf<UItemStaticData> InItemStaticDataClass)
{
    FInventoryListItem& Item = Items.AddDefaulted_GetRef();
    Item.ItemInstance = NewObject<UInventoryItemInstance>();
    Item.ItemInstance->Init(InItemStaticDataClass);
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
