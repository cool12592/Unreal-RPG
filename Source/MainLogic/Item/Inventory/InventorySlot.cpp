// Fill out your copyright notice in the Description page of Project Settings.


#include "InventorySlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "MyPlayerController.h"
#include "Kismet/GameplayStatics.h" 

void UInventorySlot::RefreshSlot(int32 itemID, int32 itemCount, UDataTable* itemDataTable)
{
    ItemID = itemID;
    ItemCount = itemCount;

    if (ItemID <= 0)
    {
        SettingEmptySlot();
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
        if (ItemTexture && TableItemInfo->ItemThumbnail)
        {
            ItemTexture->SetBrushFromTexture(TableItemInfo->ItemThumbnail);
            ItemTexture->SetVisibility(ESlateVisibility::Visible);
        }

        if (ItemCountText)
        {
            ItemCountText->SetText(FText::AsNumber(ItemCount));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("ItemInfo for ID %d not found in DataTable"), ItemID);
    }
}

void UInventorySlot::UseSlotItem()
{
    if (ItemID <= 0)
        return;

    if (OnItemUseDelegate.IsBound())
    {
        OnItemUseDelegate.Execute(ItemInfo.ItemType);
    }
}

void UInventorySlot::SettingEmptySlot()
{
    if (ItemTexture)
    {
        ItemTexture->SetVisibility(ESlateVisibility::Hidden);
    }

    if (ItemCountText)
    {
        ItemCountText->SetText(FText::AsNumber(ItemCount));
    }
}
