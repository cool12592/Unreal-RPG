// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopSlot.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "ShopWidget.h"

void UShopSlot::InitSlot(int32 itemID, UDataTable* itemDataTable)
{
    if (isInit)
        return;
    isInit = true;

    ItemID = itemID;

    if (ItemID <= 0)
    {
        ItemCost->SetText(FText::GetEmpty());
        ItemImage->SetVisibility(ESlateVisibility::Hidden);
        return;
    }

    if (!itemDataTable)
    {
        UE_LOG(LogTemp, Warning, TEXT("ItemDataTable is not assigned in %s"), *GetName());
        return;
    }

    FString RowName = FString::Printf(TEXT("%d"), ItemID);
    FItem_Info* TableItemInfo = itemDataTable->FindRow<FItem_Info>(*RowName, TEXT(""));
    ItemInfo = *TableItemInfo;

    if (TableItemInfo)
    {
        if (ItemImage && TableItemInfo->ItemThumbnail)
        {
            ItemImage->SetBrushFromTexture(TableItemInfo->ItemThumbnail);
        }

        if (ItemCost)
        {
            int32 GoldValue = TableItemInfo->ItemGold;
            FString CostString = FString::Printf(TEXT("%d Gold"), GoldValue);
            ItemCost->SetText(FText::FromString(CostString));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("ItemInfo for ID %d not found in DataTable"), ItemID);
    }
}

void UShopSlot::CancelClick()
{
    if (SelectImage)
    {
        SelectImage->SetVisibility(ESlateVisibility::Hidden);
    }
}

void UShopSlot::HandleSlotClicked()
{
    if (ItemID <= 0)
        return;
    if (TrySelectSlotDelegate.IsBound() && !TrySelectSlotDelegate.Execute(this))
        return;

    if (SelectImage)
    {
        SelectImage->SetVisibility(ESlateVisibility::Visible);
    }
}
