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
