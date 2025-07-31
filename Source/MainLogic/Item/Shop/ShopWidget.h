// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemBase.h"
#include "ShopWidget.generated.h"


DECLARE_DELEGATE_RetVal_OneParam(int32, FOnGetShopItemID, int32);
DECLARE_DELEGATE_RetVal_TwoParams(bool, FOnTryPurchaseItem, int32 /*BuyCount*/, const FItem_Info& /*ItemInfo*/);

/**
 * 
 */
UCLASS()
class NOTEBOOK_API UShopWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void NativeConstruct();
	
	UFUNCTION(BlueprintCallable)
	void OnBuyButtonClicked(int BuyCount);
	bool TrySelectSlot(class UShopSlot* shopSlot);
	
	FOnGetShopItemID OnGetShopItemIDDelegate;
	FOnTryPurchaseItem OnTryPurchaseItemDelegate;

	UPROPERTY(BlueprintReadWrite)
	bool isShowPayCheck = false;


protected:
	UFUNCTION(BlueprintImplementableEvent)
	void HandlePurchaseSuccessView();
	UFUNCTION(BlueprintImplementableEvent)
	void HandlePurchaseFailView();
	UPROPERTY(BlueprintReadWrite)
	class UShopSlot* CurSelectSlot;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UShopSlot> ShopSlotClass;
	UPROPERTY(EditAnywhere)
	UDataTable* ItemDataTable;
	UPROPERTY(meta = (BindWidget))
	class UGridPanel* GridPanel_ShopItemSlot;

private:
	static constexpr int32 TotalItems = 18;
	static constexpr int32 Rows = 6;
	static constexpr int32 Cols = 6;


};
