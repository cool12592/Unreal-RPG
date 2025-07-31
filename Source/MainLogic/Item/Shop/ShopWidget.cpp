// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopWidget.h"
#include "ShopSlot.h"
#include "Kismet/GameplayStatics.h" 
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"

void UShopWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    for (int32 Index = 0; Index < TotalItems; Index++)
    {
        int32 ItemID = OnGetShopItemIDDelegate.IsBound() ? OnGetShopItemIDDelegate.Execute(Index) : 0;

        if (!ShopSlotClass) continue;

        UShopSlot* ShopSlot = CreateWidget<UShopSlot>(this, ShopSlotClass);
        if (!ShopSlot) continue;

        ShopSlot->InitSlot(ItemID, ItemDataTable);
        ShopSlot->TrySelectSlotDelegate.BindUObject(this, &UShopWidget::TrySelectSlot);

        int32 Row = Index / Rows;
        int32 Column = Index % Cols;

        if (GridPanel_ShopItemSlot)
        {
            UPanelSlot* PanelSlot = GridPanel_ShopItemSlot->AddChild(ShopSlot);
            if (UGridSlot* GridSlot = Cast<UGridSlot>(PanelSlot))
            {
                GridSlot->SetRow(Row);
                GridSlot->SetColumn(Column);
            }
        }
    }
}

bool UShopWidget::TrySelectSlot(UShopSlot* shopSlot)
{
    if (isShowPayCheck)
        return false;
	if (!shopSlot)
		return false;

	if (CurSelectSlot)
		CurSelectSlot->CancelClick();

	CurSelectSlot = shopSlot;
    return true;
}

void UShopWidget::OnBuyButtonClicked(int BuyCount)
{
    if (!CurSelectSlot)
        return;

    if (OnTryPurchaseItemDelegate.IsBound())
    {
        // 구매 시도 (결과에 따라 뷰 처리)
        if (OnTryPurchaseItemDelegate.Execute(BuyCount, CurSelectSlot->ItemInfo))
        {
            HandlePurchaseSuccessView();
        }
        else
        {
            HandlePurchaseFailView();
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("OnTryPurchaseItemDelegate is not bound."));
    }
}




