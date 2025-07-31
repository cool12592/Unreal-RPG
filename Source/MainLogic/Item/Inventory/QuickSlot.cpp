// Fill out your copyright notice in the Description page of Project Settings.


#include "QuickSlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "MyPlayerController.h"
#include "Kismet/GameplayStatics.h" 
#include "InventorySlot.h"

void UQuickSlot::LinkQuickSlot(UInventorySlot* InvenSlot)
{
    if (!InvenSlot)
        return;

    ItemID = InvenSlot->ItemID;
    ItemCount = InvenSlot->ItemCount;
    ItemInfo = InvenSlot->ItemInfo;

    if (ItemID <= 0)
        return;

    if (ItemTexture)
    {
        ItemTexture->SetBrushFromTexture(ItemInfo.ItemThumbnail);
        ItemTexture->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
    }

    if (ItemCountText)
    {
        ItemCountText->SetText(FText::AsNumber(ItemCount));
    }
}

void UQuickSlot::CopyQuickSlot(UQuickSlot* ThereQuickSlot)
{
    if (!ThereQuickSlot)
        return;

    ItemID = ThereQuickSlot->ItemID;
    ItemCount = ThereQuickSlot->ItemCount;
    ItemInfo = ThereQuickSlot->ItemInfo;

    if (ItemID <= 0)
        return;

    if (ItemTexture)
    {
        ItemTexture->SetBrushFromTexture(ItemInfo.ItemThumbnail);
        ItemTexture->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
    }

    if (ItemCountText)
    {
        ItemCountText->SetText(FText::AsNumber(ItemCount));
    }
}

void UQuickSlot::RefreshQuickSlot(ItemType ItemType, int32 Count)
{
    if (ItemID <= 0)
        return;

    if (ItemType != ItemInfo.ItemType)
        return;

    ItemCount = Count;

    if (ItemCountText)
    {
        ItemCountText->SetText(FText::AsNumber(ItemCount));
    }
}

void UQuickSlot::UseQuickSlotItem()
{
    if (ItemID <= 0)
        return;

    if (OnItemUseDelegate.IsBound())
    {
        OnItemUseDelegate.Execute(ItemInfo.ItemType);
    }
}


