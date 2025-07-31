// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryWidget.h"
#include "Components/TextBlock.h"
#include "QuickSlot.h"
#include "InventorySlot.h"

void UInventoryWidget::RefreshGold()
{
    if (OnGoldRefreshRequested.IsBound())
    {
        int32 Gold = OnGoldRefreshRequested.Execute();
        if (GoldText)
        {
            GoldText->SetText(FText::AsNumber(Gold));
        }
    }
}

void UInventoryWidget::RefreshInvenSlot()
{
    OnInvenSlotRefreshRequested.ExecuteIfBound();
}

void UInventoryWidget::ChangeQuickSlots(TArray<class UQuickSlot*> NewQuickSlotArray)
{
    if (QuickSlotArray == NewQuickSlotArray)
        return;

    for (int i = 0; i < QuickSlotArray.Num() && i < NewQuickSlotArray.Num(); i++)
    {
        NewQuickSlotArray[i]->CopyQuickSlot(QuickSlotArray[i]);
    }
    
    QuickSlotArray = NewQuickSlotArray;
    BindAllQuickSlotUseItemDelegate();
}

void UInventoryWidget::BindAllInvenSlotUseItemDelegate()
{
    for (int i = 0; i < InvenSlotArray.Num(); i++)
    {
        InvenSlotArray[i]->OnItemUseDelegate = OnItemUseDelegate;
    }
}

void UInventoryWidget::BindAllQuickSlotUseItemDelegate()
{
    for (int i = 0; i < QuickSlotArray.Num(); i++)
    {
        QuickSlotArray[i]->OnItemUseDelegate = OnItemUseDelegate;
    }
}

void UInventoryWidget::RefreshInventory()
{
    RefreshInvenSlot();
    RefreshGold();
    SetEquipUI();
}




 

